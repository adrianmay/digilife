#include <pthread.h>
#include <string.h>
#include "types.h"
#include "misc/h.h"
#include "globals/h.h"
#include "XXBomb_meap/2.h"
#include "XX_pile/2.h"
#include "h.h"

#if ZZ
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static void lock() { pthread_mutex_lock(&mutex); }
static void unlock() { pthread_mutex_unlock(&mutex); }
#else
static void lock() {}
static void unlock() {}
#endif

static void show(void) {
  meapOfXXBombs.show();
  pileOfXXs.show(false);
}

static bool open() {
  lock();
  meapOfXXBombs.open();
  bool virgin = pileOfXXs.open();
  unlock();
  return virgin;
}

static void close(FATE fate) {
  lock();
  pileOfXXs.close(fate);
  meapOfXXBombs.close(fate);
  unlock();
}

static Ix count(void) {
  return pileOfXXs.count();
}

static XX * get(XXIx i) {
  return pileOfXXs.get(i);
}

//Below, the trailing underscore means no lock taken

// Updates bomp expiry and reorders meap. 
// Assumes collectRent was just called.
static bool updateDeathWithXXAndBomb_(XX* p, XXBomb * pBomb) {
  Cash cash = p->rent.cash;
  Tocks ttl = cash / ( tockPrice() * (sizeof(XX)+sizeof(XXBomb)) );
  //printf("UpdateDeath: ttl=%d\n", ttl);
  Tocks death = tocksNow() + ttl;
  meapOfXXBombs.check();
  // This changes bomb time and reorders meap:
  bool res = meapOfXXBombs.editTocksWhenLocked(p->rent.bomb, death);
  meapOfXXBombs.check();
  return res;
  // The meap is now properly reordered but nobody called kill
}

// Wrappers:
static bool updateDeathWithXX_(XX * pXX) {
  XXBombIx iBomb = pXX->rent.bomb;
  XXBomb * pBomb = pileOfXXBombs.get(iBomb);
  return updateDeathWithXXAndBomb_(pXX, pBomb);
}

// Debug:
//static XXIx bombee;
//static void bombeeSafe(Ix i, void * p) { 
//  XXBomb * pB = (XXBomb *) p;
//  if (pB->who.i == bombee.i) {
//    printf("After chomp: Another bomb for XX %d\n", bombee.i);
//    abort();
//  }
//}
//static void bombeeSafe2(Ix i, void * p) { 
//  XXBomb * pB = (XXBomb *) p;
//  if (pB->who.i == bombee.i) {
//    printf("In alloc: Another bomb for XX %d\n", bombee.i);
//    abort();
//  }
//}

void showXXBomb(XXBombIx i, XXBomb * p) {
  printf("tocks=%d,who=%d\n", p->tocks, p->who.i);
}

static void kill_(void) {
  XXBomb bomb; // Bomb copied out to here
  Tocks now = tocksNow();
  while (true) { // Returns when nothing to kill for now
    bomb.who = badXXIx; // Prevent false alarms
    meapOfXXBombs.check();
    Chomped ch = meapOfXXBombs.chomp(now, &bomb, 0);
    //showXXBomb(badXXBombIx, &bomb);
    meapOfXXBombs.check();
    if (ch == Killed ) {
      //bombee = bomb.who;
      //meapOfXXBombs.forAll(bombeeSafe);
      meapOfXXBombs.check();
      // For raffle, we don't free it, so this line is wrong:
      //pileOfXXs.free(bomb.who); //TODO: funeral and recover cash
      onXXKilled(bomb.who); // DOES need to free the block
      meapOfXXBombs.check();
      //printf("Killed XX %i\n", bomb.who.i);
      continue;
    }
    if (ch == Extinct) {
      //printf("XXs are extinct\n");
      onXXsExtinct(); 
      meapOfXXBombs.check();
      return;
    }
    return; // Must be Idle
  }
}

static void kill(void) {
  lock();
  kill_();
  unlock();
}

static bool isGod(XX * pXX) { return pXX->rent.bomb.i == BAD_INDEX; }
// Kills it if it has non-positive cash
// Assume rent just collected so cash is up to date
static void review_(XX * pXX) {
  //printf("Hotel reviewing i=%d\n", pXX->rent.bomb.i);
  if (isGod(pXX))
    return;
  meapOfXXBombs.check();
  updateDeathWithXX_(pXX);
  meapOfXXBombs.check();
  kill_();
}

static void review(XXIx i) {
  lock();
  XX * pXX = get(i);
  review_(pXX);
  unlock();
}

//Deduct cash and set last paid to now
static void collectRent_(XX * pXX, Cash * collected, Cash * defaulted) {
  XXRent * pRent = &pXX->rent;
  Tocks now = tocksNow();
  pRent->lastPaidRent = now;
  Tocks timeUnpaid = now - pRent->lastPaidRent;
  Cash bill = tockPrice() * (sizeof(XX)+sizeof(XXBomb)) * timeUnpaid;
  if (pRent->cash >= bill) {
    pRent->cash -= bill;
    *collected = bill;
    *defaulted = 0;
  } else {
    Cash c = pRent->cash;
    pRent->cash = 0;
    *collected = c;
    *defaulted = bill-c;
  }
} // Should now update death in bomb

static void collectRent(XXIx i) {
  lock();
  XX * pXX = get(i);
  Cash collected, defaulted;
  collectRent_(pXX, &collected, &defaulted);
  review_(pXX);
  unlock();
  if (collected) onXXRentCollected(collected);
  if (defaulted) onXXRentDefaulted(defaulted);
}
 
static void enrich_(XX * pXX, Cash amt) {
  //printf("Transfer: amt=%'ld, iFrom=%d, iTo=%d\n", amt, iFrom.i, iTo.i);
  XXRent * pRent = &pXX->rent;
  pRent->cash += amt;
}

static void enrich(XXIx i, Cash amt) {
  lock();
  enrich_(get(i), amt);
  unlock();
}

static bool chargeIfCan_(XX * pXX, Cash amt) {
  //printf("Transfer: amt=%'ld, iFrom=%d, iTo=%d\n", amt, iFrom.i, iTo.i);
  XXRent * pRent = &pXX->rent;
  bool g = isGod(pXX);
  if (g) 
    pRent->cash -= amt;
  else { // Not a god
    if (pRent->cash < amt) 
      return false;
    pRent->cash -= amt;
    review_(pXX);
    meapOfXXBombs.check();
  }
  return true;
}

static bool chargeIfCan(XXIx i, Cash amt) {
  bool res = false;
  lock();
  res = chargeIfCan_(get(i), amt);
  unlock();
  return res;
}

static Cash rob_(XX * pXX) {
  if (isGod(pXX))
    return 0;
  Cash c = pXX->rent.cash;
  if (c>0)
    enrich_(pXX, -c);
  meapOfXXBombs.check();
  review_(pXX);
  meapOfXXBombs.check();
  return c;
}

static Cash rob(XXIx i) {
  Cash c;
  lock();
  c = rob_(get(i));
  unlock();
  return c; //TODO: proper accounts
}

void showXX(XXIx i, XX * p) {
  printf("ix=%4d|nick=%x,lastPaidRent=%d,cash=%'ld,bomb=%d,", i.i, p->rent.nick, p->rent.lastPaidRent, p->rent.cash, p->rent.bomb.i);
  showXXBody(i, &p->body);
}

static XXIx alloc_(Cash cash, XX ** pp, bool * pRecycled) {
  XX * p;
  XXIx i = pileOfXXs.alloc(&p, pRecycled);
  if (cash == 0) { 
    p->rent.lastPaidRent = tocksNow();
    p->rent.cash = 0;
    p->rent.bomb = badXXBombIx;
    pileOfXXs.modUsr(1);
  } else {
    p->rent.lastPaidRent = tocksNow();
    p->rent.cash = cash;
    XXBombIx iBomb;
    XXBomb * pBomb;
    meapOfXXBombs.check();
    meapOfXXBombs.insert(&iBomb, &pBomb, i.i); // onNew should do the rest
    meapOfXXBombs.check();
  }
  if (pp) *pp = p;
  return i;
}

static XXIx alloc(Cash cash, XX ** pp, bool * pRecycled) {
  lock();
  XXIx ret = alloc_(cash, pp, pRecycled);
  unlock();
  return ret;
}

// Assumes some mutex is held, despite the name
// That's true because onNewXX only called from meap's insert
//   which for the hotel is only called from hotel's alloc
// We know it exists, and it doesn't have or need money
void onNewXXBomb(XXBombIx iBomb, Ix hint) {
  //printf("OnNew: iBomb: %d hint: %d\n", iBomb.i, hint);
  XXBomb * pBomb = pileOfXXBombs.get(iBomb);
  pBomb->who = (XXIx){hint};
  XX * p = pileOfXXs.get(pBomb->who);
  p->rent.bomb = iBomb;
  updateDeathWithXXAndBomb_(p, pBomb);
  meapOfXXBombs.check();
}

// Similarly thread safe already, I think?
void onMoveXXBomb(XXBomb * pBomb, XXBombIx to) {
  //printf("Moving bomb for bulk %d from %d to %d\n", pBomb->who.i, p->rent.bomb.i, to.i);
  XX * p = pileOfXXs.get(pBomb->who);
  p->rent.bomb = to;
  meapOfXXBombs.check();
}

static void forAll(XXPileAction act) { pileOfXXs.forAll(false, act); }

XXHotel hotelOfXXs = { open, alloc, get, enrich, chargeIfCan, collectRent, review
                     , forAll, rob, kill, count, close, show, showXX
                     , sizeof(XX)+sizeof(XXBomb) };

