#include <string.h>
#include <stdio.h>
#include "pile.h"
#include "meap.h"

Index parent(Index i) {return (i-1)/2;}
Index left  (Index i) {return 2*i + 1;}
Index right (Index i) {return 2*i + 2;}

void swap(Pilehead * ph, MeapCallbacks * mc, Index i1, Index i2) {
  void * p1 = findInPile(ph, i1);
  void * p2 = findInPile(ph, i2);
  memcpy(mc->tmp, p1, ph->rec);
  memcpy(p1, p2, ph->rec);
  memcpy(p2, mc->tmp, ph->rec);
  mc->onMove(p1, i2);
  mc->onMove(p2, i1);
}

// Returns whether or not root changed
void siftUp(Pilehead * ph, MeapCallbacks * mc, Index iCur) {
  if (iCur == 0) return;
  Score sCur;
  while (iCur > 0) {
    sCur = mc->onScore(findInPile(ph, iCur));
    Index iPar = parent(iCur);
    Score sPar = mc->onScore(findInPile(ph, iPar));
    if (sPar <= sCur) return;
    swap(ph, mc, iCur, iPar);
    iCur = iPar;
  }
  mc->onNewLow(sCur);
}

void siftDown(Pilehead * ph, MeapCallbacks * mc, Index iCur) {
  Index cnt = getUsr(ph);
  while (1) {
    Index iL = left(iCur);
    Index iR = right(iCur);
    Index iSmallest = iCur;
    Score sCur = mc->onScore(findInPile(ph, iCur));
    Score sL =  mc->onScore(findInPile(ph, iL));
    Score sR =  mc->onScore(findInPile(ph, iR));
    Score sSmallest = sCur;
    if (iL < cnt && sL < sSmallest) { iSmallest = iL; sSmallest = sL; }
    if (iR < cnt && sR < sSmallest) iSmallest = iR;
    if (iSmallest == iCur) break;
    swap(ph, mc, iCur, iSmallest);
    iCur = iSmallest;
  } // end of while
}

void meapInsert(Pilehead * ph, MeapCallbacks * mc, void * proto) {
  Index iSlot;
  Index meapTop = getUsr(ph);
  if (meapTop < ph->top) 
    iSlot = meapTop;
  else
    iSlot = allocInPile(ph, proto, 0, 0);
  modUsr(ph, 1);
  void * pSlot = findInPile(ph, iSlot);
  mc->onMove(pSlot, iSlot);
  if (iSlot>0) siftUp(ph, mc, iSlot);
}

void meapRemove(Pilehead * ph, MeapCallbacks * mc, Index iCur) {
  Score sLowOrig = mc->onScore(findInPile(ph, 0));
  Index iLast = getUsr(ph)-1;
  swap(ph, mc, iLast, iCur);
  modUsr(ph, -1);
  siftDown(ph, mc, iCur);
  siftUp(ph, mc, iCur);
  Score sLowNow = mc->onScore(findInPile(ph, 0));
  if (sLowNow != sLowOrig) mc->onNewLow(sLowNow);
}

