#include <stdio.h>
#include <string.h>
#include "meap.h"
#include "time.h" // Just for printing slowly

Index parent(Index i) {return (i-1)/2;}
Index left  (Index i) {return 2*i + 1;}
Index right (Index i) {return 2*i + 2;}

void swap(Pilehead * ph, MeapCallbacks * mc, Index i1, Index i2) {
  if (i1==i2) return;
  void * p1 = findInPile(ph, i1);
  void * p2 = findInPile(ph, i2);
  memcpy(mc->tmp, p1, ph->rec);
  memcpy(p1, p2, ph->rec);
  memcpy(p2, mc->tmp, ph->rec);
  mc->onMove(p1, i2);
  mc->onMove(p2, i1);
}

bool siftUp(Pilehead * ph, MeapCallbacks * mc, Index iCur) {
  if (iCur == 0) return false;
  Score sCur;
  while (iCur > 0) {
    sCur = mc->getScore(findInPile(ph, iCur));
    Index iPar = parent(iCur);
    Score sPar = mc->getScore(findInPile(ph, iPar));
    if (sPar <= sCur) return false;
    swap(ph, mc, iCur, iPar);
    iCur = iPar;
  }
  return true;
}

void siftDown(Pilehead * ph, MeapCallbacks * mc, Index iCur) {
  Index cnt = getUsr(ph);
  while (1) {
    Index iL = left(iCur);
    Index iR = right(iCur);
    Index iSmallest = iCur;
    Score sCur = mc->getScore(findInPile(ph, iCur));
    Score sL   = mc->getScore(findInPile(ph, iL));
    Score sR   = mc->getScore(findInPile(ph, iR));
    Score sSmallest = sCur;
    if (iL < cnt && sL < sSmallest) { iSmallest = iL; sSmallest = sL; }
    if (iR < cnt && sR < sSmallest) iSmallest = iR;
    if (iSmallest == iCur) break;
    swap(ph, mc, iCur, iSmallest);
    iCur = iSmallest;
  } // end of while
}

bool meapInsert(Pilehead * ph, MeapCallbacks * mc, void ** pNew, uint32_t hint) {
  Index iSlot;
  Index meapTop = getUsr(ph);
  if (meapTop < ph->top) {
    iSlot = meapTop;
    *pNew = findInPile(ph, iSlot);
  }  
  else
    iSlot = allocInPile(ph, pNew, 0, 0);
  mc->onNew(iSlot, hint);
  modUsr(ph, 1);
  mc->onMove(*pNew, iSlot);
  if (iSlot>0) 
    return siftUp(ph, mc, iSlot); 
  return false;
}

bool meapReview(Pilehead * ph, MeapCallbacks * mc, Index iCur) {
  siftDown(ph, mc, iCur);
  return siftUp(ph, mc, iCur);
}

bool meapRemove(Pilehead * ph, MeapCallbacks * mc, Index iCur) {
  Index iLast = getUsr(ph)-1;
  swap(ph, mc, iLast, iCur);
  modUsr(ph, -1);
  return meapReview(ph, mc, iCur);
}
  
int chomp(Pilehead * ph, MeapCallbacks * mc, Score thresh, void * out, int outlen) {
  uint32_t u = getUsr(ph); 
  sleepS_(1);
  if (u==0) {
    return -1;
  }
  void * p = findInPile(ph, 0);
  Score sZ = mc->getScore(p);
  if (sZ<thresh) {
    memcpy(out, p, outlen);
    meapRemove(ph, mc, 0);
    return 1; // We don't care if the lowest score changed or not. 
  }             // Just want to know if there's more to kill.
  memcpy(out, p, outlen);
  return 0;
}

