
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "types.h"
#include "misc/h.h"
#include "args/h.h"
#include "h.h"
#include "structs.h"
#define MAX_FULL_PATH (MAX_FILENAME+DATA_DIR_MAX)

Ix recLen(Pilehead * ph) { return ph->rec; }

// The minimum stretch of virtual memory that can accomodate a pile with
//  a known record size and number of slots reserved (not necessarily in use).
size_t pileSize(Pilehead * ph) {
  int x = ph->rec * ph->res + sizeof(Pilehead),
      d = x/PAGE,
      r = x%PAGE;
  return PAGE*(d + (r?1:0)); //Round up to whole page
}

// How many slots in a pile of a given filesize, given a known record size
uint32_t capacity(Pilehead * ph, size_t filelen) {
  return (filelen-sizeof(Pilehead)) / ph->rec;
}

Pilehead * openPile(const char * basefilename, uint32_t rec, uint32_t stp, Ix lim, bool * virgin) { // returns array address
  char filename[MAX_FULL_PATH];
  snprintf(filename, MAX_FULL_PATH, "%s/%s", getDataDir(), basefilename);
  if (rec<4) { printf("Record size too small for free indices.\n"); quit(1); }
  // Could this ^ be at compile time?
  int fd;
  *virgin=false;
  if (filename[0]) {
    fd = open(filename, O_RDWR | O_APPEND);
    if (fd<0) {
      *virgin=true;
      fd = open(filename, O_RDWR | O_CREAT | O_APPEND, S_IRUSR|S_IWUSR);
      ftruncate(fd, PAGE*stp);
    }
    if (fd<0) { fprintf(stdout, "Can't open file %s cos of %d\n", filename, fd); quit(1); }
  } else { // allowing memory-only piles for test and forget
    fd = -1;
    *virgin = true;
  }
  void * reserve;
  reserve = mmap(0, ((uint64_t)lim)*PAGE, PROT_NONE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
  if (reserve == (void*)-1) { fprintf(stdout, "Can't make reserve mapping\n"); quit(1);  }
  void * filemap;
  if (fd!=-1) {
    filemap = mmap(reserve, fileSize(fd), PROT_READ|PROT_WRITE, MAP_SHARED_VALIDATE|MAP_FIXED, fd, 0);
  } else {
    filemap = mmap(reserve, PAGE*stp, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_FIXED, fd, 0);
  }
  if (filemap == (void*)-1) { fprintf(stdout, "Can't make file mapping\n"); quit(1);  }
  Pilehead * ph = (Pilehead*) filemap;
  ph->fd = fd;
  if (*virgin) {
    ph->hdr = sizeof(Pilehead);
    ph->rec = rec;
    ph->stp = stp;
    ph->lim = lim;
    ph->res = (PAGE*stp - sizeof(Pilehead))/rec;
    ph->top = 0;
    ph->fri = BAD_INDEX;
    ph->fro = 0x7FFFFFFF;
    ph->frn = 0;
    ph->usr = 0;
    strncpy(ph->fn, basefilename, MAX_FILENAME);
  }
  return ph;
}

void closePile(Pilehead * ph, FATE fate) {
  if (!ph) return;
  int fd = ph->fd;
  if (fd == -1) return;
  close(fd);
  char src[MAX_FULL_PATH];
  snprintf(src, MAX_FULL_PATH, "%s/%s", getDataDir(), ph->fn);
  if (fate==DELETE) unlink(src);
  if (fate==HIDE) {
    char dest[MAX_FULL_PATH+1];
    snprintf(dest, MAX_FULL_PATH+1, "%s/.%s", getDataDir(), ph->fn);
    rename(src, dest);
  }
  munmap(ph, ((uint64_t)ph->lim)*PAGE);
}

// Overstepped the high watermark so grow
void growPile(Pilehead * ph) {
  if (ph->res > ph->top) return;
  size_t oldLen = pileSize(ph);
  size_t newLen = oldLen + ph->stp*PAGE;
  ph->res = capacity(ph, newLen);
  if (ph->fd != -1) ftruncate(ph->fd, newLen);
  void * wholeMap = (void*)ph;
  void * newPh = mmap(wholeMap+oldLen, newLen-oldLen, PROT_READ|PROT_WRITE, MAP_SHARED_VALIDATE|MAP_FIXED, ph->fd, oldLen);
  if (newPh != wholeMap + oldLen) {
    printf("Subsequent mmap failed returning %p instead of %p with wholeMap=%p changing length from %ld to %ld\n",
           newPh, ph, wholeMap, oldLen, newLen);
    quit(1);
  }
}

typedef struct {Ix nextFree; } Free;
// The main way to dereference an index:
void * findInPile(Pilehead * ph, Ix i) { return (((void*)(ph+1)) + i*ph->rec); }
// If it's free, we know we're pointing at an index of another free block or BAD_INDEX, so cast it:
Free * findFreeInPile(Pilehead * ph, Ix i) { return (Free*) findInPile(ph,i); }

void * withInPile(Pilehead * ph, Ix i, F f, void * u) {
  void * p = findInPile(ph, i);
  void * ret = f(p, u);
  return ret;
}
                                                //
// Allocate a new slot by trying the free list, or incrementing top, or growing
Ix allocInPile(Pilehead * ph, void ** ppNew, bool * pRecycled, void * ghost, int ghostlen) {
  Ix ret;
  // fro NEVER has the top bit set
  // nextFree=BAD_INDEX         : not a used block or a free block
  // nextFree has top bit set   : lower bits are a free block index
  // nextFree has top bit clear : lower bits are the nick of a used block

  if (ph->fro != 0x7FFFFFFF) { // There are free blocks
    if (pRecycled) *pRecycled = true;
    ph->frn -= 1;
    ret = ph->fro;
    if (ret & 0x80000000) abort();
    Free * pFree = findFreeInPile(ph,ret);
    printf("In allocInPile: Setting fro of %s from %d to %d from nextFree of %d\n", ph->fn, ph->fro, pFree->nextFree & 0x7FFFFFFF, ret);
    ph->fro = pFree->nextFree & 0x7FFFFFFF;
    if (ph->fro == 0x7FFFFFFF)
      ph->fri = BAD_INDEX;
    if (ghost) { // If desired, save whatever was in the block after nextFree
      void * pGhost = pFree + 1; // while it was free
      memcpy(ghost, pGhost, ghostlen);
    }
  } else {
    if (pRecycled) *pRecycled = false;
    growPile(ph);
    ph->top++;
    ret = ph->top-1;
  }
  Free * pNew = findFreeInPile(ph, ret);
  // Make up a new nick. Anything with MSB set but not BAD_INDEX. In Rent, the nick is declared there.
  Ix nck = randIntBelow(0x7FFFFFFF);
  printf("In allocInPile: Setting nextFree of %d in %s to nick %x\n", ret, ph->fn, nck);
  pNew->nextFree = nck; // Bombs can overwrite it.
  if (ppNew) *ppNew = pNew;
  return ret;
}

// Free a block to the free list
// There's no rent collector thread so this needs MT protection, but higher up
void freeInPile(Pilehead * ph, Ix i, void * ghost, int ghostlen) {
  if (i & 0x80000000) abort();
  Free * pFree = findFreeInPile(ph,i); // Get the block
  if (pFree->nextFree & 0x80000000) {
    printf("DOUBLE FREE in %s: %d\n", ph->fn, i);
    abort();
  }
  printf("In freeInPile 1: Setting nextFree of %d in %s to %x\n", i, ph->fn, BAD_INDEX);
  pFree->nextFree = BAD_INDEX;
  if (ph->fri != BAD_INDEX) {
    Free * pOldInEnd = findFreeInPile(ph,ph->fri); // Get the block
    printf("In freeInPile 2: Setting nextFree of %d in %s to %x\n", ph->fri, ph->fn, i | 0x8000000);
    pOldInEnd->nextFree = i | 0x80000000; // Point old in end at newly freed block
  }
  ph->fri = i; //Set free in end to that block
  if (ghost)  //Stuff something into the free block after nextFree if desired
    memcpy((void*)(pFree+1), ghost, ghostlen); 
  if (ph->fro==0x7FFFFFFF) {
    printf("In freeInPile: Setting fro to %x\n", i);
    ph->fro = i & 0x7FFFFFFF; // Only if this is the first do we mess with the out end
  }
  ph->frn += 1;
  // Should assert that fri and fro have same badness
}

// Sundry utils:
Ix countFree(Pilehead * ph ) { return ph->frn; }
Ix countPop(Pilehead * ph ) { return ph->top - ph->frn; }
Ix getUsr(Pilehead * ph) { return ph->usr; }
void setUsr(Pilehead * ph, Ix u) { ph->usr = u; }
void modUsr(Pilehead * ph, int32_t u)  { ph->usr += u; }


void forAllPile(Pilehead * ph, bool onlyToUsr, PileAction act) {
  if (!ph) {
    printf("Pile is closed\n");
    abort();
  }
  for (Ix i=0;i<(onlyToUsr?ph->usr:ph->top);i++) {
    Free * p = findFreeInPile(ph, i);
    if (p->nextFree & 0x80000000); 
    else act(i);
  }
}

// TODO: Could be done with forAllPile:
void showPile(Pilehead * ph, VIP showSlot, bool onlyToUsr) {
  printf("\nPILE: %s\n", ph?ph->fn:"closed.");
  if (!ph) {
    printf("Pile is closed\n");
    abort();
  }
  printf("  REC |   TOP |   USR |   FRN |   FRI |   FRO \n");
  printf("%5d | %5d | %5d | %5d | %5d | %5d\n\n", ph->rec, ph->top, ph->usr, ph->frn, ph->fri, ph->fro);
  for (Ix i=0;i<(onlyToUsr?ph->usr:ph->top);i++) {
    Free * p = findFreeInPile(ph, i);
    printf("%5d | ",i);
    if (p->nextFree & 0x80000000) 
      printf("Free: nextFree=%4d | ", p->nextFree & 0x7FFFFFFF);
    showSlot(i, p);
  }
}
