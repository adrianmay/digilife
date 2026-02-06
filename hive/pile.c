// This is the type-agnostic implementation of a memory-mapped-file-backed fixed-size heap

#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "pile.h"

#define PAGE 4096

typedef int (*cb)();
int quit(int i) {exit(i);} // { return *((int*)(0)); }

int fileSize(int fd) {
  struct stat sb;
  if (fstat(fd, &sb) == -1) { printf("Can't stat fd=%d\n", fd); quit(1); }
  return sb.st_size;
}

size_t pileSize(Pilehead * ph, uint32_t rec) {
  int x = rec * ph->res + sizeof(Pilehead),
      d = x/PAGE,
      r = x%PAGE;
  return PAGE*(d + (r?1:0)); //Round up to whole page
}

uint32_t capacity(Pilehead * ph, uint32_t rec, size_t filelen) {
  return (filelen-sizeof(Pilehead)) / rec;
}

void * openPileInternal(Str filename, uint32_t rec, uint32_t preamble, uint32_t stp, uint64_t lim) { // returns array address
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
  reserve = mmap(0, lim*PAGE, PROT_NONE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
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
    ph->res = (PAGE*stp - sizeof(Pilehead))/rec;
    ph->top = 0;
    ph->fre = BAD_INDEX;
  }
  printf("Opened file map at %p\n",filemap);
  return filemap+sizeof(Pilehead);
}

void grow(Pilehead * ph, uint32_t rec, uint32_t preamble, uint32_t stp) {
  if (ph->res > ph->top) return;
  size_t oldLen = pileSize(ph, rec);
  size_t newLen = oldLen + stp*PAGE;
  ph->res = capacity(ph, rec, newLen);
  if (ph->fd != -1) ftruncate(ph->fd, newLen);
  void * wholeMap = (void*)ph;
  void * newPh = mmap(wholeMap+oldLen, newLen-oldLen, PROT_READ|PROT_WRITE, MAP_SHARED_VALIDATE|MAP_FIXED, ph->fd, oldLen);
  if (newPh != wholeMap + oldLen) {
    printf("Subsequent mmap failed returning %p instead of %p with wholeMap=%p changing length from %d to %d\n", newPh, ph, wholeMap, oldLen, newLen);
    quit(1);
  }
}

void  * findInternal  (Pilehead * ph, uint32_t rec, uint32_t i) { return (((void*)(ph+1)) + i*rec); }
uint32_t * findFreeOnPile(Pilehead * ph, uint32_t rec, uint32_t i) { return (uint32_t*) findInternal(ph,rec,i); }

uint32_t allocInternal(Pilehead * ph, uint32_t rec, uint32_t preamble, uint32_t stp, char * prototype, void * ghost, int ghostlen) {
  uint32_t ret;
  if (ph->fre != BAD_INDEX) {
    ret = ph->fre;
    uint32_t * pFree = findFreeOnPile(ph,rec,ret);
    ph->fre = *pFree;
    if (ghost) {
      uint32_t * pGhost = pFree+1; 
      memcpy(ghost, (void*) pGhost, ghostlen);
    }
  } else {
    grow(ph, rec, preamble, stp);
    ph->top++;
    ret = ph->top-1;
  }
  char * pNew = findInternal(ph, rec, ret);
  memcpy(pNew, prototype, rec);
  return ret;
}

void freeInternal(Pilehead * ph, uint32_t i, uint32_t rec, uint32_t preamble, void * ghost, int ghostlen) {
  uint32_t * pFree = findFreeOnPile(ph,rec,i);
  memset((void*)pFree,0xaa,rec);
  *pFree = ph->fre;
  ph->fre = i;
  memcpy((void*)(pFree+1), ghost, ghostlen);
}

// Nice to have explicit population count

uint32_t countFree_(Pilehead * ph, uint32_t rec, uint32_t i, int sofar) {
  uint32_t ni = *(uint32_t*)(((void*)(ph+1)) + rec*i);
  if (ni == BAD_INDEX) return sofar;
  return countFree_(ph, rec, ni, sofar+1);
}

uint32_t countFree(Pilehead * ph, uint32_t rec ) {
  if (ph->fre==BAD_INDEX) return 0;
  return countFree_(ph, rec, ph->fre, 1);
}

//////////////////////////


