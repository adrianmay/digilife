#include <pthread.h>
#include <signal.h>
#include "types.h"
#include "misc/h.h"
#include "globals/h.h"
#include "XXBomb_meap/2.h"
#include "XXBulk_pile/2.h"
#include "h.h"

XXBulkIndex rentCollectorIndexXX = (XXBulkIndex) {0}; // This is crap

static void close(FATE fate) {
  pileOfXXBulks.close(fate);
  meapOfXXBombs.close(fate);
}

static Index count(void) {
  return pileOfXXBulks.count();
}

static XXBulk * get(XXBulkIndex i) { return pileOfXXBulks.get(i);}

static bool updateXXDeath(XXBulk* pBulk, XXBomb * pBomb) {
  Cash cash = pBulk->rent.cash;
  Tocks ttl = cash/tockPrice();
  Tocks death = tocksNow() + ttl;
  return meapOfXXBombs.editTocksWhenLocked(pBulk->rent.bomb, death);
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

static void review(XXBulkIndex i) { 
  updateXXDeathWithBulkIndex(i); 
}

static void collectRent(XXBulkIndex i) {
  updateTocks();
  XXBulk * pBulk = pileOfXXBulks.get(i);
  XXRent * pRent = &pBulk->rent;
  Tocks time = tocksNow() - pRent->lastPaidRent;
  Cash bill = tockPrice() * time;
  pRent->cash -= bill;
  pRent->lastPaidRent = tocksNow();
}

static void transfer(Cash amt, XXBulkIndex iFrom, XXBulkIndex iTo) {
  collectRent(iFrom);
  XXBulk * pFrom = pileOfXXBulks.get(iFrom);
  XXRent * pFromRent = &pFrom->rent;
  XXBulk * pTo = pileOfXXBulks.get(iTo);
  XXRent * pToRent = &pTo->rent;
  pFromRent->cash -= amt;
  pToRent->cash += amt;
  collectRent(iTo);
  review(iFrom);
  review(iTo);
}

static XXBulkIndex alloc(Cash cash, XXBulk ** ppBulk, bool * pRecycled) {
  XXBulk * pBulk;
  XXBulkIndex iBulk = pileOfXXBulks.alloc(&pBulk, pRecycled);
  pBulk->rent.cash = cash;
  updateTocks();
  pBulk->rent.lastPaidRent = tocksNow();
  XXBombIndex iBomb;
  XXBomb * pBomb;
  meapOfXXBombs.insert(&iBomb, &pBomb, iBulk.i); // onNew should do the rest
  if (ppBulk) *ppBulk = pBulk;
  return iBulk;
}

static bool open(Cash cash, XXBulkIndex * pI) {
  meapOfXXBombs.open();
  bool virgin = pileOfXXBulks.open();
  XXBulkIndex i = alloc(cash, 0, 0);
  if (pI) *pI = i;
  return virgin;
}

// This has to get called at strategic times from worker threads
static void killer(void) {
  XXBomb bomb; // Bomb copied out to here
  Tocks now = tocksNow();            
  while (true) { // Returns when nothing to kill for now
    bomb.who = badXXBulkIndex; // Prevent false alarms
    Chomped ch = meapOfXXBombs.chomp(now, &bomb);
    if (ch == Killed ) { pileOfXXBulks.free(bomb.who); continue; }
    if (ch == Extinct) { onXXsExtinct(); return; }
    return; // Must be Idle
  }
}

static Cash rob(XXBulkIndex i) { 
  XXBulk * pBulk = pileOfXXBulks.get(i);
  XXRent * pRent = &pBulk->rent;
  transfer(pRent->cash, i, rentCollectorIndexXX);
  killer();
  return 0; //TODO: proper accounts
}

static void show(void) {
  meapOfXXBombs.show();
  pileOfXXBulks.show();
}

void onNewXXBomb(XXBombIndex iBomb, Index hint) { 
  //printf("OnNew: iBomb: %d hint: %d\n", iBomb.i, hint);
  XXBomb * pBomb = pileOfXXBombs.get(iBomb);
  pBomb->who = (XXBulkIndex){hint};
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

XXHotel hotelOfXXs = {open, alloc, get, transfer, review, rob, killer, count, close, show};
