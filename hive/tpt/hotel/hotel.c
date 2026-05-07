#include "types.h"
#include "misc/h.h"
#include "globals/h.h"
#include "XXBulk_pile/1.h"
#include "XXBomb_pile/1.h"
#include "XXBomb_pile/XXBomb.h"
#include "XXBulk_pile/XXBulk.h"
#include "XXBomb_meap/2.h"
//#include "XXBulk_pile/2.h"
#include <pthread.h>
#include <signal.h>
#include "h.h"


static void open() {
  pileOfXXBulks.open();
  pileOfXXBombs.open();
}

static void close(FATE fate) {
  pileOfXXBulks.close(fate);
  pileOfXXBombs.close(fate);
}

static bool updateXXDeath(XXBulk* pBulk, XXBomb * pBomb) {
  Cash cash = pBulk->rent.cash;
  Tocks ttl = cash/tockPrice();
  Tocks death = tocksNow() + ttl;
  return meapOfXXBombs.editWhenWhenLocked(pBulk->rent.bomb, death);
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

static Cash rob(XXBulkIndex i) { 
  return 0; //TODO: writeme
}

static XXBulkIndex alloc(Cash cash, XXBulk ** ppBulk) {
  XXBulkIndex iBulk = pileOfXXBulks.alloc(ppBulk);
  (*ppBulk)->rent.cash = cash;
  updateTocks();
  (*ppBulk)->rent.lastPaidRent = tocksNow();
  XXBombIndex iBomb;
  XXBomb * pBomb;
  meapOfXXBombs.insert(&iBomb, &pBomb, iBulk.i); // onNew should do the rest
  return iBulk;
}

// This has to get called at strategic times from worker threads
static void killer() {
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

static void show() {
  meapOfXXBombs.show();
  pileOfXXBulks.show();
}

void onNewXXBomb(XXBombIndex iBomb, Index hint) { 
  printf("OnNew: iBomb: %d hint: %d\n", iBomb.i, hint);
  XXBomb * pBomb = pileOfXXBombs.get(iBomb);
  pBomb->who = (XXBulkIndex){hint};
  updateXXDeathWithBulkIndexAndBombPointer(pBomb->who, pBomb);
}

void onMoveXXBomb(XXBomb * pBomb, XXBombIndex to) { 
  XXBulk * pBulk = pileOfXXBulks.get(pBomb->who);
  pBulk->rent.bomb = to;
}

void showXXBomb(XXBomb * p) {
  printf("when=%d,who=%d\n", p->when, p->who.i);
}

void showXXBulk(XXBulk * p) {
  printf("cash=%'ld,lastPaidRent=%d,bomb=%d,", p->rent.cash, p->rent.lastPaidRent, p->rent.bomb.i);
  showXXBody(&p->body);
}

XXHotel hotelOfXXs = {open, alloc, review, rob, killer, close, show};
