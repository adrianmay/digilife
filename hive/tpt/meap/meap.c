#include <string.h>
#include "misc/h.h"
#include "2.h"

static XXIx parent(XXIx i) {return ( XXIx ){ (i.i-1)/2 };}
static XXIx left  (XXIx i) {return ( XXIx ){ 2*i.i + 1 };}
static XXIx right (XXIx i) {return ( XXIx ){ 2*i.i + 2 };}

static void swap(XXIx i1, XXIx i2) {
  if (i1.i==i2.i) return;
  XX tmp;
  size_t len = sizeof(tmp);
  XX * p1 = pileOfXXs.get(i1);
  XX * p2 = pileOfXXs.get(i2);
  memcpy(&tmp, p1, len);
  memcpy(p1, p2, len);
  memcpy(p2, &tmp, len);
  onMoveXX(p1, i1);
  onMoveXX(p2, i2);
}

static bool siftUp(XXIx iCur) {
  if (iCur.i == 0) return false;
  Score sCur;
  while (iCur.i > 0) {
    sCur = pileOfXXs.get(iCur)->tocks;
    XXIx iPar = parent(iCur);
    XX * pPar = pileOfXXs.get(iPar);
    Score sPar = pPar->tocks;
    if (sPar <= sCur) return false;
    swap(iCur, iPar);
    iCur.i = iPar.i;
  }
  return true; //We changed the zeroth member
}

// We can't use free cos meaps must be contiguous, so we use the heap's usr for the "meapish" size
static void siftDown(XXIx iCur) {
  Ix cnt = pileOfXXs.getUsr();
  while (1) {
    XXIx iSmallest = iCur;
    XX * pCur = pileOfXXs.get(iCur); Score sCur = pCur->tocks;
    Score sSmallest = sCur;

    XXIx iL = left(iCur);
    if (iL.i<cnt) {
      XX * pL   = pileOfXXs.get(iL  ); Score sL   =   pL->tocks;
      if (iL.i < cnt && sL < sSmallest) { iSmallest.i = iL.i; sSmallest = sL; }
      XXIx iR = right(iCur);
      if (iR.i<cnt) {
        XX * pR   = pileOfXXs.get(iR  ); Score sR   =   pR->tocks;
        if (iR.i < cnt && sR < sSmallest) iSmallest.i = iR.i;
      }
    }  

    if (iSmallest.i == iCur.i) break;
    swap(iCur, iSmallest);
    iCur = iSmallest;
  } // end of while
}

// Returns whether or not the lowest changed.
static bool insert(XXIx * pI, XX ** ppNew, Ix hint) {
  Ix meapTop = pileOfXXs.getUsr(); //meapish size
  if (meapTop < pileOfXXs.count()) { // That's pile's top minus pile's free count. But we'll never use free in this pile anyway.
    pI->i = meapTop;                   // Got meapish spares so just return one
    *ppNew = pileOfXXs.get(*pI);    
  }  
  else
    *pI = pileOfXXs.alloc(ppNew, 0); 
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

static bool editTocksWhenLocked(XXIx iCur, Score when) { 
  //printf("editTocksWhenLocked: i=%d, when=%d\n", iCur.i, when);
  pileOfXXs.get(iCur)->tocks = when;
  siftDown(iCur); 
  bool res = siftUp(iCur); 
  return res;
}

static bool editTocksTakingLock(XXIx iCur, Score when) { 
  bool res = editTocksWhenLocked(iCur, when);
  return res;
}

static bool erase_(XXIx iCur) {
  Ix cnt = pileOfXXs.getUsr();
  if (!cnt) {
    return false;
  }
  Ix iLast = cnt-1;
  swap((XXIx){iLast}, iCur);
  pileOfXXs.modUsr(-1);
  siftDown(iCur); //Not calling review cos I'd need a recursive mutex
  return siftUp(iCur);
}

static bool erase(XXIx iCur) {
  int res = erase_(iCur);
  return res;
}

static void show(void) {
  printf("MEAP:\n");
  pileOfXXs.show(true);
}

static Chomped chomp(Score thresh, XX * pCopyOut, int pseudoAnimals) {
  Chomped res;
  Ix x = meapOfXXs.size();
  if (x<=pseudoAnimals) { res = Extinct; }
  else {
    XXIx i = (XXIx) {0};
    XX * p = pileOfXXs.get(i);
    memcpy(pCopyOut, p, sizeof(XX));
    Score lowestScoreInMeap = p->tocks;
    ScoreDiff sd = wrapSub32S(lowestScoreInMeap, thresh);
    //printf("chomped: lowest=%d, thresh=%d, sd=%d\n", lowestScoreInMeap, thresh, sd);
    if (sd <= 0) {
      erase_(i);
      //show();
      res = Killed;
    } else {
      res = Idle;
    }
  }
  return res;
}

static bool checkOrdered(void) {
  bool ok=true;
  Ix cnt = pileOfXXs.getUsr();
  for (Ix i=1;i<cnt;i++) {
    XXIx iCur = (XXIx){i};
    Score sCur = pileOfXXs.get(iCur)->tocks;
    XXIx iPar = parent(iCur);
    Score sPar = pileOfXXs.get(iPar)->tocks;
    ok &= sPar <= sCur;
  }
  return ok;
}

static Ix size(void) {  return pileOfXXs.getUsr(); } 

static bool open(void) { return pileOfXXs.open(); }
static void close(FATE f) { pileOfXXs.close(f); }

XXMeap meapOfXXs = { open, close, insert, editTocksWhenLocked, editTocksTakingLock, erase, chomp, checkOrdered, size, show };
