#include <fcntl.h>                                   
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>                                
#include <sys/stat.h>                                
#include <unistd.h>

#include "pile.h"

// typedef int (*cb)();
int quit(int i) {exit(i);} // { return *((int*)(0)); }                         
    
int fileSize(int fd) {                                                         
  struct stat sb;                                                              
  if (fstat(fd, &sb) == -1) { printf("Can't stat fd=%d\n", fd); quit(1); }
  return sb.st_size;
} 
  
size_t pileSize(Pilehead * ph) {
  int x = ph->rec * ph->res + sizeof(Pilehead),                                    
      d = x/PAGE,
      r = x%PAGE;                                                              
  return PAGE*(d + (r?1:0)); //Round up to whole page
} 
  
uint32_t capacity(Pilehead * ph, size_t filelen) {
  return (filelen-sizeof(Pilehead)) / ph->rec;
}

#define GLOBALS_FILENAME "Globals.pile"

void * openGlobals_(uint64_t len, bool * virgin) {
  *virgin=false;
  int fd = open(GLOBALS_FILENAME, O_RDWR | O_APPEND);
  if (fd<0) {
    *virgin=true;
    fd = open(GLOBALS_FILENAME, O_RDWR | O_CREAT | O_APPEND, S_IRUSR|S_IWUSR);
    ftruncate(fd, len);
  }
  if (fd<0) { fprintf(stderr, "Can't open file %s cos of %d\n", GLOBALS_FILENAME, fd); quit(1); }
  void * reserve = mmap(0, len, PROT_NONE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
  if (reserve == (void*)-1) { fprintf(stderr, "Can't make reserve mapping\n"); quit(1); }
  void * filemap = mmap(reserve, fileSize(fd), PROT_READ|PROT_WRITE, MAP_SHARED_VALIDATE|MAP_FIXED, fd, 0);
  if (filemap == (void*)-1) { fprintf(stderr, "Can't make file mapping\n"); quit(1);  }
  return filemap;
}

void closeGlobals_(int fd, bool rm) {
  if (fd == -1) return;
//  munmap(ph);
  close(fd);
  if (rm) unlink(GLOBALS_FILENAME);
}

Pilehead * openPile(const char * filename, uint32_t rec, uint32_t stp, Index lim) { // returns array address
  if (rec<4) { printf("Record size too small for free indices.\n"); quit(1); }
  // Could this ^ be at compile time?
  int fd;
  bool virgin=false;
  if (filename[0]) {
    fd = open(filename, O_RDWR | O_APPEND);
    if (fd<0) {
      virgin=true;
      fd = open(filename, O_RDWR | O_CREAT | O_APPEND, S_IRUSR|S_IWUSR);
      ftruncate(fd, PAGE*stp);
    }
    if (fd<0) { fprintf(stderr, "Can't open file %s cos of %d\n", filename, fd); quit(1); }
  } else { // allowing memory-only piles for test and forget
    fd = -1;
    virgin = true;
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
  if (virgin) {
    ph->hdr = sizeof(Pilehead);
    ph->rec = rec;
    ph->stp = stp;
    ph->lim = lim;
    ph->res = (PAGE*stp - sizeof(Pilehead))/rec;
    ph->top = 0;
    ph->fre = BAD_INDEX;
    ph->frn = 0;
    ph->usr = 0;
    strncpy(ph->fn, filename, MAX_FILENAME-1);
  }
  return ph;
}

void closePile(Pilehead * ph, bool rm) {
  int fd = ph->fd;
  if (fd == -1) return;
//  munmap(ph);
  close(fd);
  if (rm) unlink(ph->fn);
}

void hidePile(Pilehead * ph) {
  char dest[MAX_FILENAME+1];
  *dest='.';
  strcpy(dest+1,ph->fn);
  rename(ph->fn, dest);
}

void growPile(Pilehead * ph) {
  if (ph->res > ph->top) return;
  size_t oldLen = pileSize(ph);
  size_t newLen = oldLen + ph->stp*PAGE;
  ph->res = capacity(ph, newLen);
  if (ph->fd != -1) ftruncate(ph->fd, newLen);
  void * wholeMap = (void*)ph;
  void * newPh = mmap(wholeMap+oldLen, newLen-oldLen, PROT_READ|PROT_WRITE, MAP_SHARED_VALIDATE|MAP_FIXED, ph->fd, oldLen);
  if (newPh != wholeMap + oldLen) {
    printf("Subsequent mmap failed returning %p instead of %p with wholeMap=%p changing length from %d to %d\n", newPh, ph, wholeMap, oldLen, newLen);
    quit(1);
  }
}

void  * findInPile(Pilehead * ph, Index i) { return (((void*)(ph+1)) + i*ph->rec); }                                         
Index * findFreeInPile(Pilehead * ph, Index i) { return (Index*) findInPile(ph,i); }

Index allocInPile(Pilehead * ph, void ** pNew, void * ghost, int ghostlen) {
  Index ret;
  if (ph->fre != BAD_INDEX) {
    ret = ph->fre;
    Index * pFree = findFreeInPile(ph,ret);
    ph->fre = *pFree;
    ph->frn--;
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
  
void freeInPile(Pilehead * ph, Index i, void * ghost, int ghostlen) {
  Index * pFree = findFreeInPile(ph,i);                                                                                                
  memset((void*)pFree,0xaa,ph->rec);
  *pFree = ph->fre;
  ph->fre = i;
  ph->frn++;
  memcpy((Index*)(pFree+1), ghost, ghostlen);
} 
  
Index countFree(Pilehead * ph ) { return ph->frn; }
Index countPop(Pilehead * ph ) { return ph->top - ph->frn; }      
Index getUsr(Pilehead * ph) { return ph->usr; }
void setUsr(Pilehead * ph, Index u) { ph->usr = u; } 
void modUsr(Pilehead * ph, int32_t u)  { ph->usr += u; } 


