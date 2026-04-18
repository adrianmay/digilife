
#include <fcntl.h>                                   
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>                                
#include <sys/stat.h>                                
#include <unistd.h>
#include "types.h"
#include "misc/h.h"
#include "h.h"
#include "structs.h"


Index recLen(Pilehead * ph) { return ph->rec; }

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

Pilehead * openPile(const char * filename, uint32_t rec, uint32_t stp, Index lim, bool * virgin) { // returns array address
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
    if (fd<0) { fprintf(stderr, "Can't open file %s cos of %d\n", filename, fd); quit(1); }
  } else { // allowing memory-only piles for test and forget
    fd = -1;
    *virgin = true;
  }
  void * reserve;
  reserve = mmap(0, ((uint64_t)lim)*PAGE, PROT_NONE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
  if (reserve == (void*)-1) { fprintf(stderr, "Can't make reserve mapping\n"); quit(1);  }
  void * filemap;
  if (fd!=-1) {
    filemap = mmap(reserve, fileSize(fd), PROT_READ|PROT_WRITE, MAP_SHARED_VALIDATE|MAP_FIXED, fd, 0);
  } else {
    filemap = mmap(reserve, PAGE*stp, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_FIXED, fd, 0);
  }
  if (filemap == (void*)-1) { fprintf(stderr, "Can't make file mapping\n"); quit(1);  }
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
    ph->fro = BAD_INDEX;
    ph->frn = 0;
    ph->usr = 0;
    strncpy(ph->fn, filename, MAX_FILENAME-1);
    for (int a=0;a<LIKE_FREE;a++) 
      freeInPile(ph, allocInPile(ph, 0, 0, 0), 0, 0);
  }
  return ph;
}

void closePile(Pilehead * ph, FATE fate) {
  if (!ph) return;
  int fd = ph->fd;
  if (fd == -1) return;
  close(fd);
  if (fate==DELETE) unlink(ph->fn);
  if (fate==HIDE) {
    char dest[MAX_FILENAME+1];
    *dest='.';
    strcpy(dest+1,ph->fn);
    rename(ph->fn, dest);
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

// The main way to dereference an index:
void  * findInPile(Pilehead * ph, Index i) { return (((void*)(ph+1)) + i*ph->rec); }                                         
// If it's free, we know we're pointing at an index of another free block or BAD_INDEX, so cast it:
Index * findFreeInPile(Pilehead * ph, Index i) { return (Index*) findInPile(ph,i); }

void * withInPile(Pilehead * ph, Index i, F f, void * u) {
  void * p = findInPile(ph, i);
  void * ret = f(p, u);
  return ret;
}
                                                //
// Allocate a new slot by trying the free list, or incrementing top, or growing
// Should be in a mutex
Index allocInPile(Pilehead * ph, void ** pNew, void * ghost, int ghostlen) {
  Index ret;
  if (ph->fro != BAD_INDEX && ph->frn > LIKE_FREE) {
    atomic_fetch_sub(&ph->frn, 1);           
    // Take mutex
    ret = ph->fro;
    Index * pFree = findFreeInPile(ph,ret);
    ph->fro = *pFree;
    // Release mutex
    if (ghost) { 
      Index * pGhost = pFree+1;
      memcpy(ghost, (void*) pGhost, ghostlen);
    }
  } else {
    growPile(ph);                                                                                                             
    ph->top++; 
    ret = ph->top-1;
  } 
  if (pNew) *pNew = findInPile(ph, ret);
  return ret;
}   
  
// Free a block to the free list
// Only the rent collector thread does this?
void freeInPile(Pilehead * ph, Index i, void * ghost, int ghostlen) {
  Index * pFree = findFreeInPile(ph,i); // Get the block
  memset((void*)pFree,0xaa,ph->rec); // Erase for privacy
  *pFree = BAD_INDEX;
  if (ph->fri != BAD_INDEX) {
    Index * pOldInEnd = findFreeInPile(ph,ph->fri); // Get the block
    *pOldInEnd = i; // Point old in end at newly freed block                                                 
  }
  ph->fri = i; //Set free in end to that block
  memcpy((Index*)(pFree+1), ghost, ghostlen); //Stuff the ghost into the rest
  if (ph->fro==BAD_INDEX) ph->fro = i; // Only if this is the first do we mess with the out end
  atomic_fetch_add(&ph->frn, 1);           
  // Should assert that fri and fro have same badness
} 
  
// Sundry utils:
Index countFree(Pilehead * ph ) { return ph->frn; }
Index countPop(Pilehead * ph ) { return ph->top - ph->frn; }      
Index getUsr(Pilehead * ph) { return ph->usr; }
void setUsr(Pilehead * ph, Index u) { ph->usr = u; } 
void modUsr(Pilehead * ph, int32_t u)  { ph->usr += u; } 


