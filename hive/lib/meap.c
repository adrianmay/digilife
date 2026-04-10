#include <stdio.h>
#include <string.h>
#include "meap.h"
#include "sleep.h" // Just for printing slowly

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

// Returns whether or not the root changed.
bool meapInsert(Pilehead * ph, MeapCallbacks * mc, Index * pI, void ** pNew, uint32_t hint) {
  Index meapTop = getUsr(ph); // We can't use the real free cos the meap must remain contiguous, so we have our own count of active meaps.
  if (meapTop < ph->top) { 
    *pI = meapTop;
    *pNew = findInPile(ph, *pI);
  }  
  else
    *pI = allocInPile(ph, pNew, 0, 0);
  mc->onNew(*pI, hint); // Expected to stuff *pI with things that depend on hint
  modUsr(ph, 1);
  mc->onMove(*pNew, *pI); //Expected to keep track of where the meap is.
  if (*pI>0) 
    return siftUp(ph, mc, *pI); // Will call onMove if it moves this meap.
  return false;
}

bool meapReview(Pilehead * ph, MeapCallbacks * mc, Index iCur) {
  siftDown(ph, mc, iCur);
  return siftUp(ph, mc, iCur);
}

bool meapRemove(Pilehead * ph, MeapCallbacks * mc, Index iCur) {
  Index cnt = getUsr(ph);
  if (!cnt) return false;
  Index iLast = cnt-1;
  swap(ph, mc, iLast, iCur);
  modUsr(ph, -1);
  return meapReview(ph, mc, iCur);
}
  
// Returns -1 if empty, 1 if it killed something, 0 just returned the champ without changes
int chomp(Pilehead * ph, MeapCallbacks * mc, Score thresh, void * out, int outlen) {
  uint32_t u = getUsr(ph); 
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

