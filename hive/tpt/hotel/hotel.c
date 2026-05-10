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

static void review(XXBulkIndex i) { 
  collectRent(i);
  //printf("Reviewing i=%d... ", i.i);
  updateXXDeathWithBulkIndex(i); 
}

static void transfer(Cash amt, XXBulkIndex iFrom, XXBulkIndex iTo) {
  collectRent(iFrom);
  XXBulk * pFrom = pileOfXXBulks.get(iFrom);
  XXRent * pFromRent = &pFrom->rent;
  XXBulk * pTo = pileOfXXBulks.get(iTo);
  XXRent * pToRent = &pTo->rent;
  printf("Transfer before: cash: from: %'ld, to: %'ld\n", pFromRent->cash, pToRent->cash);
  pFromRent->cash -= amt;
  pToRent->cash += amt;
  printf("Transfer after:  cash: from: %'ld, to: %'ld\n", pFromRent->cash, pToRent->cash);
  collectRent(iTo);
  review(iFrom);
  review(iTo);
  printf("Transfer end:    cash: from: %'ld, to: %'ld\n", pFromRent->cash, pToRent->cash);
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
    transfer(cash, iDonor, iBulk);
  }
  XXBombIndex iBomb;
  XXBomb * pBomb;
  meapOfXXBombs.insert(&iBomb, &pBomb, iBulk.i); // onNew should do the rest
  if (ppBulk) *ppBulk = pBulk;
  return iBulk;
}

static XXBulkIndex alloc(Cash cash, XXBulkIndex iDonor, XXBulk ** ppBulk, bool * pRecycled) {
  if (!pileOfXXBulks.indexValid(iDonor)) {
    printf("Don't disregard double entry\n");
    exit(1);
  }
  return alloc_(cash, iDonor, ppBulk, pRecycled);
}

static bool open(Cash cash, XXBulkIndex * pI) {
  meapOfXXBombs.open();
  bool virgin = pileOfXXBulks.open();
  if (virgin) {
    XXBulkIndex i = alloc_(cash, (XXBulkIndex){BAD_INDEX}, 0, 0);
    if (pI) *pI = i;
  }
  return virgin;
}

static void show(void) {
  meapOfXXBombs.show();
  pileOfXXBulks.show(false);
}

// This has to get called at strategic times from worker threads
static void killer(void) {
  XXBomb bomb; // Bomb copied out to here
  updateTocks();
  Tocks now = tocksNow();            
  while (true) { // Returns when nothing to kill for now
    bomb.who = badXXBulkIndex; // Prevent false alarms
    Chomped ch = meapOfXXBombs.chomp(now, &bomb, 1);
    if (ch == Extinct) { onXXsExtinct(); return; }
    if (ch == Killed ) { 
      pileOfXXBulks.free(bomb.who); //TODO: funeral and recover cash
      printf("Killing %i\n", bomb.who.i);
      show();
      continue; 
    }
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

XXHotel hotelOfXXs = {open, alloc, get, transfer, review, rob, killer, count, close, show};

