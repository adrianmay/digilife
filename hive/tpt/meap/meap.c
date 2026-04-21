#include <string.h>
#include "1.h"
#include "2.h"

static XXIndex parent(XXIndex i) {return ( XXIndex ){ (i.i-1)/2 };}
static XXIndex left  (XXIndex i) {return ( XXIndex ){ 2*i.i + 1 };}
static XXIndex right (XXIndex i) {return ( XXIndex ){ 2*i.i + 2 };}

static void swap(XXIndex i1, XXIndex i2) {
  if (i1.i==i2.i) return;
  XX tmp;
  size_t len = sizeof(tmp);
  XX * p1 = pileOfXXs.get(i1);
  XX * p2 = pileOfXXs.get(i2);
  memcpy(&tmp, p1, len);
  memcpy(p1, p2, len);
  memcpy(p2, &tmp, len);
  onMoveXX(p1, i2);
  onMoveXX(p2, i1);
}

static bool siftUp(XXIndex iCur) {
  if (iCur.i == 0) return false;
  Score sCur;
  while (iCur.i > 0) {
    sCur = getXXScore(pileOfXXs.get(iCur));
    XXIndex iPar = parent(iCur);
    XX * pPar = pileOfXXs.get(iPar);
    Score sPar = getXXScore(pPar);
    if (sPar <= sCur) return false;
    swap(iCur, iPar);
    iCur.i = iPar.i;
  }
  return true;
}

// We can't use free cos meaps must be contiguous, so we use the heap's usr for the "meapish" size
static void siftDown(XXIndex iCur) {
  Index cnt = pileOfXXs.getUsr();
  while (1) {
    XXIndex iL = left(iCur);
    XXIndex iR = right(iCur);
    XXIndex iSmallest = iCur;
    XX * pCur = pileOfXXs.get(iCur); Score sCur = getXXScore(pCur);
    XX * pL   = pileOfXXs.get(iL  ); Score sL   = getXXScore(pL  );
    XX * pR   = pileOfXXs.get(iR  ); Score sR   = getXXScore(pR  );
    Score sSmallest = sCur;
    if (iL.i < cnt && sL < sSmallest) { iSmallest.i = iL.i; sSmallest = sL; }
    if (iR.i < cnt && sR < sSmallest) iSmallest.i = iR.i;
    if (iSmallest.i == iCur.i) break;
    swap(iCur, iSmallest);
    iCur = iSmallest;
  } // end of while
}

// Returns whether or not the lowest changed.
static bool insert(XXIndex * pI, XX ** ppNew, Index hint) {
  Index meapTop = pileOfXXs.getUsr(); //meapish size
  if (meapTop < pileOfXXs.count()) { // That's pile's top minus pile's free count. But we'll never use free in this pile anyway.
    pI->i = meapTop;                   // Got meapish spares so just return one
    *ppNew = pileOfXXs.get(*pI);    
  }  
  else
    *pI = pileOfXXs.alloc(ppNew); 
  // ppNew is now correct either way.
  onNewXX(*pI, hint);                   // Expected to stuff *pI with things that depend on hint. Needed after allocing the meap member but before sorting.
  pileOfXXs.modUsr(1);                        // Not sure if this should be before the above, but certainly it's before siftUp. 
  if (pI->i > 0) {                          // No point sorting a singleton.
    bool res = siftUp(*pI);             // Calls siftUp if it returns true;
    if (!res) onMoveXX(*ppNew, *pI);    // Something else might want to keep track of where the meap member is.
    return res;
  }
  return false;
}

static bool review(XXIndex iCur) { siftDown(iCur); return siftUp(iCur); }

static bool erase(XXIndex iCur) {
  Index cnt = pileOfXXs.getUsr();
  if (!cnt) return false;
  Index iLast = cnt-1;
  swap((XXIndex){iLast}, iCur);
  pileOfXXs.modUsr(-1);
  return review(iCur);
}


static bool checkOrdered() {
  bool ok=true;
  Index cnt = pileOfXXs.getUsr();
  for (Index i=1;i<cnt;i++) {
    XXIndex iCur = (XXIndex){i};
    Score sCur = getXXScore(pileOfXXs.get(iCur));
    XXIndex iPar = parent(iCur);
    Score sPar = getXXScore(pileOfXXs.get(iPar));
    ok &= sPar <= sCur;
  }
  return ok;
}

XXMeap meapOfXXs = { insert, review, erase, checkOrdered };
