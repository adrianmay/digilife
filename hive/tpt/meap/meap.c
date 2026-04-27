#include <string.h>
#include "1.h"
#include "2.h"
#include "misc/h.h"

pthread_mutex_t XXMeapMutex = PTHREAD_MUTEX_INITIALIZER;
static void lock(bool b, int line) {
  printf(b ? "Locking %d\n" : "Unlocking %d\n", line);
  if (b) pthread_mutex_lock(&XXMeapMutex);
  else   pthread_mutex_unlock(&XXMeapMutex);
}

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
    sCur = pileOfXXs.get(iCur)->YY;
    XXIndex iPar = parent(iCur);
    XX * pPar = pileOfXXs.get(iPar);
    Score sPar = pPar->YY;
    if (sPar <= sCur) return false;
    swap(iCur, iPar);
    iCur.i = iPar.i;
  }
  return true; //We changed the zeroth member
}

// We can't use free cos meaps must be contiguous, so we use the heap's usr for the "meapish" size
static void siftDown(XXIndex iCur) {
  Index cnt = pileOfXXs.getUsr();
  while (1) {
    XXIndex iL = left(iCur);
    XXIndex iR = right(iCur);
    XXIndex iSmallest = iCur;
    XX * pCur = pileOfXXs.get(iCur); Score sCur = pCur->YY;
    XX * pL   = pileOfXXs.get(iL  ); Score sL   =   pL->YY;
    XX * pR   = pileOfXXs.get(iR  ); Score sR   =   pR->YY;
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
  lock(true, __LINE__);
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
    lock(false, __LINE__);
    return res;
  }
  lock(false, __LINE__);
  return false;
}

static bool editWhen(XXIndex iCur, Score when) { 
  lock(true, __LINE__);
  pileOfXXs.get(iCur)->YY = when;
  siftDown(iCur); 
  bool res = siftUp(iCur); 
  lock(false, __LINE__);
  return res;
}

static bool erase_(XXIndex iCur) {
  Index cnt = pileOfXXs.getUsr();
  if (!cnt) {
    lock(false, __LINE__);
    return false;
  }
  Index iLast = cnt-1;
  swap((XXIndex){iLast}, iCur);
  pileOfXXs.modUsr(-1);
  siftDown(iCur); //Not calling review cos I'd need a recursive mutex
  return siftUp(iCur);
}

static bool erase(XXIndex iCur) {
  lock(true, __LINE__);
  int res = erase_(iCur);
  lock(false, __LINE__);
  return res;
}

static Chomped chomp(Score thresh, XX * pCopyOut) {
  Chomped res;
  lock(true, __LINE__);
  Index x = meapOfXXs.size();
  if (x==0) { res = Extinct; }
  else {
    XXIndex i = (XXIndex) {0};
    XX * p = pileOfXXs.get(i);
    memcpy(pCopyOut, p, sizeof(XX));
    Score lowestScoreInMeap = p->YY;
    ScoreDiff sd = wrapSub32S(lowestScoreInMeap, thresh);
    if (sd <= 0) {
      erase_(i);
      res = Killed;
    } else {
      res = Idle;
    }
  }
  lock(false, __LINE__);
  return res;
}

static bool checkOrdered() {
  lock(true, __LINE__);
  bool ok=true;
  Index cnt = pileOfXXs.getUsr();
  for (Index i=1;i<cnt;i++) {
    XXIndex iCur = (XXIndex){i};
    Score sCur = pileOfXXs.get(iCur)->YY;
    XXIndex iPar = parent(iCur);
    Score sPar = pileOfXXs.get(iPar)->YY;
    ok &= sPar <= sCur;
  }
  lock(false, __LINE__);
  return ok;
}

static Index size() {  return pileOfXXs.getUsr(); } 

XXMeap meapOfXXs = { insert, editWhen, erase, chomp, checkOrdered, size };
