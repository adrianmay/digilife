#include "types.h"
#include "misc/h.h"
#include "globals/h.h"
#include "XXBulk_pile/1.h"
#include "XXBomb_pile/1.h"
#include "XXBomb_pile/XXBomb.h"
#include "XXBulk_pile/XXBulk.h"
#include "XXBomb_meap/2.h"
#include "XXBulk_pile/2.h"
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

static bool updateXXDeath(XXBulk* pBulk, XXBomb * pBomb, Nanosecs * pNsRel) {
  Cash cash = pBulk->rent.cash;
  Tocks ttl = cash/tockPrice();
  Tocks death = tocksNow() + ttl;
  if (pNsRel) *pNsRel = nsUntilTock(death);
  return meapOfXXBombs.editWhen(pBulk->rent.bomb, death);
}

static bool updateXXDeathWithBulkIndex(XXBulkIndex iBulk, Nanosecs * pNsRel) {
  XXBulk * pBulk = pileOfXXBulks.get(iBulk);
  XXBombIndex iBomb = pBulk->rent.bomb;
  XXBomb * pBomb = pileOfXXBombs.get(iBomb);
  return updateXXDeath(pBulk, pBomb, pNsRel);
}
  
static bool updateXXDeathWithBulkIndexAndBombPointer(XXBulkIndex iBulk, XXBomb * pBomb) {
  XXBulk * pBulk = pileOfXXBulks.get(iBulk);
  return updateXXDeath(pBulk, pBomb, 0);
}
  
static void review(XXBulkIndex i) { 
  updateXXDeathWithBulkIndex(i); 
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

static int killerLooper(Nanosecs * pNsRel) {
  Tocks deadline;
  while (true) { // This loop is a mess
    if (meapOfXXBombs.size()==0) { return QUIT; } //Extinct
    updateTocks(); // Maybe overkill if the lock below is always fast
    Tocks now = tocksNow();
    XXBombIndex iBomb = (XXBombIndex) {0};
    XXBomb * pBomb = pileOfXXBombs.get(iBomb);
    deadline = pBomb->when;
    if (deadline <= now) {
      lockXXTimer(false);
      printf("Freeing stuff: iBomb: %d, who: %d\n", iBomb.i, pBomb->who.i);
      pileOfXXBulks.free(pBomb->who);
      lockXXTimer(true);
      meapOfXXBombs.remove(iBomb);
      continue;
    }
    *pNsRel = nsUntilTock(deadline);
    return SET|WAIT;
  }

}

static void * killer(void * p) {
  loopOnMobTimer(killerLooper, 5000000000);
  return 0;
}

XXHotel hotelOfXXs = {open, alloc, review, killer, close};

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

