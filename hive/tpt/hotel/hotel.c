#include <pthread.h>
#include "types.h"
#include "misc/h.h"
#include "globals/h.h"
#include "XXBomb_meap/2.h"
#include "XXBulk_pile/2.h"
#include "h.h"

#if ZZ
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static void lock() { pthread_mutex_lock(&mutex); } 
static void unlock() { pthread_mutex_unlock(&mutex); } 
#else
static void lock() {} 
static void unlock() {} 
#endif

XXBulkIndex rentCollectorIndexXX = (XXBulkIndex) {0}; // This is crap

static void show(void) {
  meapOfXXBombs.show();
  pileOfXXBulks.show(false);
}

static void close(FATE fate) {
  lock();
  pileOfXXBulks.close(fate);
  meapOfXXBombs.close(fate);
  unlock();
}

static Index count(void) {
  return pileOfXXBulks.count();
}

static XXBulk * get(XXBulkIndex i) { 
  return pileOfXXBulks.get(i);
}

static void collectRent(XXBulkIndex i) {
  updateTocks();
  XXBulk * pBulk = pileOfXXBulks.get(i);
  XXRent * pRent = &pBulk->rent;
  Tocks now = tocksNow();
  Tocks time = now - pRent->lastPaidRent;
  Cash bill = tockPrice() * time;
  pRent->cash -= bill;
  pRent->lastPaidRent = now;
}

static bool updateXXDeath(XXBulk* pBulk, XXBomb * pBomb) {
  Cash cash = pBulk->rent.cash;
  Tocks ttl = cash/tockPrice();
  //printf("UpdateXXDeath: ttl=%d\n", ttl);
  Tocks death = tocksNow() + ttl;
  return meapOfXXBombs.editTocksWhenLocked(pBulk->rent.bomb, death);
  // The meap is properly reordered but nobody called kill
}

static bool updateXXDeathWithBulkIndex(XXBulkIndex iBulk) {
  XXBulk * pBulk = pileOfXXBulks.get(iBulk);
  XXBombIndex iBomb = pBulk->rent.bomb;
  XXBomb * pBomb = pileOfXXBombs.get(iBomb);
  return updateXXDeath(pBulk, pBomb);
}

static bool updateXXDeathWithBulkIndexAndBombPointer(XXBulkIndex iBulk, XXBomb * pBomb) {
  XXBulk * pBulk = pileOfXXBulks.get(iBulk);
  return updateXXDeath(pBulk, pBomb);
}

static void review_(XXBulkIndex i) { 
  collectRent(i);
  //printf("Reviewing i=%d... ", i.i);
  updateXXDeathWithBulkIndex(i); 
}
static void review(XXBulkIndex i) { 
  lock();
  review_(i);
  unlock();
}

static void transfer_(Cash amt, XXBulkIndex iFrom, XXBulkIndex iTo) {
  //printf("Transfer: amt=%'ld, iFrom=%d, iTo=%d\n", amt, iFrom.i, iTo.i);
  XXBulk * pFrom = pileOfXXBulks.get(iFrom);
  XXRent * pFromRent = &pFrom->rent;
  XXBulk * pTo = pileOfXXBulks.get(iTo);
  XXRent * pToRent = &pTo->rent;
  pFromRent->cash -= amt;
  pToRent->cash += amt;
}

static void transfer(Cash amt, XXBulkIndex iFrom, XXBulkIndex iTo) {
  lock();
  review_(iFrom);
  review_(iTo);
  transfer_(amt, iFrom, iTo);
  unlock();
}

static XXBulkIndex alloc_(Cash cash, XXBulkIndex iDonor, XXBulk ** ppBulk, bool * pRecycled) {
  updateTocks();
  XXBulk * pBulk;
  XXBulkIndex iBulk = pileOfXXBulks.alloc(&pBulk, pRecycled);
  pBulk->rent.lastPaidRent = tocksNow();
  if (!pileOfXXBulks.indexValid(iDonor)) 
    pBulk->rent.cash = cash; // Genesis only
  else {
    pBulk->rent.cash = 0;
    transfer_(cash, iDonor, iBulk);
    review_(iDonor);
  }
  XXBombIndex iBomb;
  XXBomb * pBomb;
  meapOfXXBombs.insert(&iBomb, &pBomb, iBulk.i); // onNew should do the rest
  //printf("In alloc near end\n");
  show();
  review_(iBulk);
  if (ppBulk) *ppBulk = pBulk;
  return iBulk;
}

static XXBulkIndex alloc(Cash cash, XXBulkIndex iDonor, XXBulk ** ppBulk, bool * pRecycled) {
  lock();
  if (!pileOfXXBulks.indexValid(iDonor)) {
    printf("Don't disregard double entry\n");
    exit(1);
  }
  XXBulkIndex ret = alloc_(cash, iDonor, ppBulk, pRecycled);
  unlock();
  return ret;
}

static bool open(Cash cash) {
  lock();
  meapOfXXBombs.open();
  bool virgin = pileOfXXBulks.open();
  if (virgin) alloc_(cash, (XXBulkIndex){BAD_INDEX}, 0, 0); 
  unlock();
  return virgin;
}

// This has to get called at strategic times from worker threads
static void kill(void) {
  lock();
  XXBomb bomb; // Bomb copied out to here
  updateTocks();
  Tocks now = tocksNow();            
  while (true) { // Returns when nothing to kill for now
    bomb.who = badXXBulkIndex; // Prevent false alarms
    Chomped ch = meapOfXXBombs.chomp(now, &bomb, 1);
    if (ch == Extinct) { 
      printf("Extinct\n");
      onXXsExtinct(); 
      unlock();
      return; 
    }
    if (ch == Killed ) { 
      pileOfXXBulks.free(bomb.who); //TODO: funeral and recover cash
      printf("Killed %i\n", bomb.who.i);
      show();
      continue; 
    }
    unlock();
    return; // Must be Idle
  }
}

static Cash rob(XXBulkIndex i) { 
  XXBulk * pBulk = pileOfXXBulks.get(i);
  XXRent * pRent = &pBulk->rent;
  transfer(pRent->cash, i, rentCollectorIndexXX);
  kill();
  return 0; //TODO: proper accounts
}

void onNewXXBomb(XXBombIndex iBomb, Index hint) { 
  //printf("OnNew: iBomb: %d hint: %d\n", iBomb.i, hint);
  XXBomb * pBomb = pileOfXXBombs.get(iBomb);
  pBomb->who = (XXBulkIndex){hint};
  XXBulk * pBulk = pileOfXXBulks.get(pBomb->who);
  pBulk->rent.bomb = iBomb;
  updateXXDeathWithBulkIndexAndBombPointer(pBomb->who, pBomb);
}

void onMoveXXBomb(XXBomb * pBomb, XXBombIndex to) { 
  XXBulk * pBulk = pileOfXXBulks.get(pBomb->who);
  //printf("Moving bomb for bulk %d from %d to %d\n", pBomb->who.i, pBulk->rent.bomb.i, to.i);
  pBulk->rent.bomb = to;
}

void showXXBomb(XXBomb * p) {
  printf("tocks=%d,who=%d\n", p->tocks, p->who.i);
}

void showXXBulk(XXBulk * p) {
  printf("cash=%'ld,lastPaidRent=%d,bomb=%d,", p->rent.cash, p->rent.lastPaidRent, p->rent.bomb.i);
  showXXBody(&p->body);
}

XXHotel hotelOfXXs = {open, alloc, get, transfer, review, rob, kill, count, close, show};

