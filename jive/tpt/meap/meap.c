#include <string.h>
#include "types.h"
#include "misc/api.h"
#include "XX_pile/ix.h"
#include "XX_pile/api.h"
#include "api.h"
#include "YY"

static pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
static void lock() { pthread_mutex_lock(&mutex); }
static void unlock() { pthread_mutex_unlock(&mutex); }

static XXIx parent(XXIx i) {return ( XXIx ){ (i.i-1)/2 };}
static XXIx left  (XXIx i) {return ( XXIx ){ 2*i.i + 1 };}
static XXIx right (XXIx i) {return ( XXIx ){ 2*i.i + 2 };}

void meapOfXXs_show(void) { printf("MEAP: "); pileOfXXs_show(false);  }

static Ix bombee;

XX * meapOfXXs_get(XXIx i) { return pileOfXXs_get(i); }

static void bombeeSafe(Ix i, void * p) { 
  return;
  Ix * pB = (Ix *) p;
  if (*pB == bombee) {
    printf("In meap: Another bomb for XX %d\n", bombee);
    meapOfXXs_show();
    //DIE("")
  }
}

void meapOfXXs_forAll(void (*cb)(Ix, void *) ) {
  Ix cnt = pileOfXXs_getUsr();
  for (Ix i=0;i<cnt;i++) cb(i, pileOfXXs_get((XXIx){i}));
}

static Ix seen[100000];

static void markOnce(Ix i, void * p) {
  Ix * pB = (Ix *) p; // Bombee
  if (seen[*pB] != BAD_INDEX) {
    printf("XX meap: checkNoDupes: Saw bombee %d in %d and %d\n", *pB, i, seen[*pB]);
    //DIE
  }
  seen[*pB]=i;
}

static void checkNoDupes() //TODO: reinstate this
{
  return; 
  memset(seen, BAD_INDEX, sizeof(seen));
  meapOfXXs_forAll(markOnce);
}

static void swap(XXIx i1, XXIx i2) {
  if (i1.i==i2.i) return;
  XX tmp;
  size_t len = sizeof(tmp);
  XX * p1 = pileOfXXs_get(i1);
  XX * p2 = pileOfXXs_get(i2);
  memcpy(&tmp, p1, len);
  memcpy(p1, p2, len);
  memcpy(p2, &tmp, len);
  onXXMeap_move(p1, i1);
  onXXMeap_move(p2, i2);
  checkNoDupes();
}

static bool siftUp(XXIx iCur) {
  bool res;
  Score sCur;
  if (iCur.i == 0) {
    res = false; 
  } else {
    while (iCur.i > 0) {
      sCur = pileOfXXs_get(iCur)->tocks;
      XXIx iPar = parent(iCur);
      XX * pPar = pileOfXXs_get(iPar);
      Score sPar = pPar->tocks;
      if (sPar <= sCur) { 
        res = false; 
        break;
      } else {
        swap(iCur, iPar);
        iCur.i = iPar.i;
        res = true;
      }
    }
  }
  checkNoDupes();
  return res;
}

// We can't use free cos meaps must be contiguous, so we use the heap's usr for the "meapish" size
static void siftDown(XXIx iCur) {
  Ix cnt = pileOfXXs_getUsr();
  while (1) {
    XXIx iSmallest = iCur;
    XX * pCur = pileOfXXs_get(iCur); Score sCur = pCur->tocks;
    Score sSmallest = sCur;

    XXIx iL = left(iCur);
    if (iL.i<cnt) {
      XX * pL   = pileOfXXs_get(iL  ); Score sL   =   pL->tocks;
      if (iL.i < cnt && sL < sSmallest) { iSmallest.i = iL.i; sSmallest = sL; }
      XXIx iR = right(iCur);
      if (iR.i<cnt) {
        XX * pR   = pileOfXXs_get(iR  ); Score sR   =   pR->tocks;
        if (iR.i < cnt && sR < sSmallest) iSmallest.i = iR.i;
      }
    }

    if (iSmallest.i == iCur.i) break;
    swap(iCur, iSmallest);
    iCur = iSmallest;
  } // end of while
  checkNoDupes();
}

// Returns whether or not the lowest changed.
bool meapOfXXs_insert(Tocks expiry, Ix hint, XXIx * pI) {
  bool res = false;
  XX * pNew;
  bombee = hint;
  meapOfXXs_forAll(bombeeSafe);
  lock();
  Ix meapTop = pileOfXXs_getUsr();    //meapish size
  if (meapTop < pileOfXXs_count()) {  // That's pile's top minus pile's free count. But we'll never use free in this pile anyway.
    pI->i = meapTop;                  // Got meapish spares so just return one
    pNew = pileOfXXs_get(*pI);
  }
  else
    *pI = pileOfXXs_alloc(&pNew, 0);
  // pNew is now correct either way.
  pNew->tocks = expiry;
  onXXMeap_new(pNew, hint);
  pileOfXXs_modUsr(1);                        // Not sure if this should be before the above, but certainly it's before siftUp.
  if (pI->i > 0) {                          // No point sorting a singleton.
    res = siftUp(*pI);             // Calls siftUp if it returns true;
  }
  unlock();
  return res;
}

bool meapOfXXs_editTocks(XXIx iCur, Score when) {
  lock();
  pileOfXXs_get(iCur)->tocks = when;
  siftDown(iCur);
  bool res = siftUp(iCur);
  checkNoDupes();
  unlock();
  return res;
}

// Assume already locked
static bool erase_(XXIx iCur) {
  Ix cnt = pileOfXXs_getUsr();
  if (!cnt) {
    return false;
  }
  XX * p = pileOfXXs_get(iCur);
  Ix * pI = (Ix *) p;
  bombee = pI[0];
  Ix iLast = cnt-1;
  swap((XXIx){iLast}, iCur);
  pileOfXXs_modUsr(-1);
  siftDown(iCur); //Not calling review cos I'd need a recursive mutex
  siftDown(iCur); //Not calling review cos I'd need a recursive mutex
  bool res = siftUp(iCur);
//  meapOfXXs_forAll(bombeeSafe);
//  checkNoDupes();
  return res;
}

bool meapOfXXs_erase(XXIx iCur) {
  lock();
  bool res = erase_(iCur);
  unlock();
  return res;
}

Chomped meapOfXXs_chomp(Score thresh, XX * pCopyOut, int pseudoAnimals) {
  Chomped res;
  lock();
  Ix x = meapOfXXs_count();
  if (x<=pseudoAnimals) { 
    //printf("chomp XX: extinct: x=%d, pseudo=%d\n", x, pseudoAnimals);
    res = Extinct; 
  }
  else {
    XXIx i = (XXIx) {0};
    XX * p = pileOfXXs_get(i);
    //if (ip[0] == 194 && ip[1] == 1138) {
    memcpy(pCopyOut, p, sizeof(XX));
    Score lowestScoreInMeap = p->tocks;
    ScoreDiff sd = wrapSub32S(lowestScoreInMeap, thresh);
    if (sd <= 0) {
      erase_(i);
      res = Killed;
    } else {
      res = Idle;
    }
  }
  checkNoDupes();
  unlock();
  return res;
}

// static bool checkSubWeights(void) {
//   Ix cnt = pileOfXXs_getUsr();
//   for (Ix i=1;i<cnt;i++) {
//     XXIx iCur = (XXIx){i};
//     XX * pCur = pileOfXXs_get(iCur);
// 
//     XXIx iL = left(iCur);
//     if (iL.i<cnt) {
//       XX * pL = pileOfXXs_get(iL);
//       if (pCur->body
//     }
// 
//     XXIx iR= right(iCur);
// 
//   }
//   return true;
// }

bool meapOfXXs_check(void) {
  bool ok=true;
  Ix cnt = pileOfXXs_getUsr();
  for (Ix i=1;i<cnt;i++) {
    XXIx iCur = (XXIx){i};
    Score sCur = pileOfXXs_get(iCur)->tocks;
    XXIx iPar = parent(iCur);
    Score sPar = pileOfXXs_get(iPar)->tocks;
    ok &= sPar <= sCur;
  }
  checkNoDupes();
  return ok;
}

Ix   meapOfXXs_count(void) { return pileOfXXs_getUsr(); }
Ix   meapOfXXs_rec(void)   { return pileOfXXs_rec(); }
bool meapOfXXs_open(void)  { return pileOfXXs_open(); }
void meapOfXXs_close(Fate f) { lock(); pileOfXXs_close(f); unlock(); }


