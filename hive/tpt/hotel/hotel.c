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

static void collectRent(XXIx i) {
  updateTocks();
  XX * p = pileOfXXs.get(i);
  XXRent * pRent = &p->rent;
  Tocks now = tocksNow();
  Tocks time = now - pRent->lastPaidRent;
  Cash bill = tockPrice() * (sizeof(XX)+sizeof(XXBomb)) * time;
  pRent->cash -= bill;
  pRent->lastPaidRent = now;
}

static bool updateXXDeath(XX* p, XXBomb * pBomb) {
  Cash cash = p->rent.cash;
  Tocks ttl = cash / ( tockPrice() * (sizeof(XX)+sizeof(XXBomb)) );
  //printf("UpdateXXDeath: ttl=%d\n", ttl);
  Tocks death = tocksNow() + ttl;
  meapOfXXBombs.check();
  bool res = meapOfXXBombs.editTocksWhenLocked(p->rent.bomb, death);
  meapOfXXBombs.check();
  return res;
  // The meap is properly reordered but nobody called kill
}

static bool updateXXDeathWithIx(XXIx i) {
  XX * p = pileOfXXs.get(i);
  XXBombIx iBomb = p->rent.bomb;
  XXBomb * pBomb = pileOfXXBombs.get(iBomb);
  return updateXXDeath(p, pBomb);
}

static bool updateXXDeathWithIxAndBombPointer(XXIx i, XXBomb * pBomb) {
  XX * p = pileOfXXs.get(i);
  return updateXXDeath(p, pBomb);
}

static void review_(XXIx i) {
  collectRent(i);
  meapOfXXBombs.check();
  //printf("Reviewing i=%d... ", i.i);
  updateXXDeathWithIx(i);
}
static void review(XXIx i) {
  lock();
  review_(i);
  unlock();
}

static XXIx bombee;
static void bombeeSafe(Ix i, void * p) { 
  XXBomb * pB = (XXBomb *) p;
  if (pB->who.i == bombee.i) {
    printf("After chomp: Another bomb for XX %d\n", bombee.i);
    abort();
  }
}
static void bombeeSafe2(Ix i, void * p) { 
  XXBomb * pB = (XXBomb *) p;
  if (pB->who.i == bombee.i) {
    printf("In alloc: Another bomb for XX %d\n", bombee.i);
    abort();
  }
}
// This has to get called at strategic times from worker threads
static void kill_(void) {
  XXBomb bomb; // Bomb copied out to here
  updateTocks();
  Tocks now = tocksNow();
  while (true) { // Returns when nothing to kill for now
    bomb.who = badXXIx; // Prevent false alarms
    meapOfXXBombs.check();
    Chomped ch = meapOfXXBombs.chomp(now, &bomb, 0);
    meapOfXXBombs.check();
    if (ch == Killed ) {
      bombee = bomb.who;
      meapOfXXBombs.forAll(bombeeSafe);
      meapOfXXBombs.check();
      onXXKilled(bomb.who);
      printf("Gonna free XX %d cos chomped\n", bomb.who.i);
      pileOfXXs.free(bomb.who); //TODO: funeral and recover cash
      meapOfXXBombs.check();
      //printf("Killed XX %i\n", bomb.who.i);
      //show();
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

static void enrich_(XXIx iWho, Cash amt) {
  //printf("Transfer: amt=%'ld, iFrom=%d, iTo=%d\n", amt, iFrom.i, iTo.i);
  XX * pWho = pileOfXXs.get(iWho);
  XXRent * pRent = &pWho->rent;
  pRent->cash += amt;
  review_(iWho);
  meapOfXXBombs.check();
  kill_(); // amt might be negative
  meapOfXXBombs.check();
}

static void enrich(XXIx iWho, Cash amt) {
  lock();
  enrich_(iWho, amt);
  unlock();
}

static bool chargeIfCan_(XXIx iWho, Cash amt) {
  //printf("Transfer: amt=%'ld, iFrom=%d, iTo=%d\n", amt, iFrom.i, iTo.i);
  XX * pWho = pileOfXXs.get(iWho);
  XXRent * pRent = &pWho->rent;
  if (pRent->cash < amt) return false;
  pRent->cash -= amt;
  review_(iWho);
  meapOfXXBombs.check();
  kill_();
  meapOfXXBombs.check();
  return true;
}

static bool chargeIfCan(XXIx iWho, Cash amt) {
  lock();
  bool res = chargeIfCan_(iWho, amt);
  unlock();
  return res;
}

static Cash rob(XXIx i) {
  lock();
  XX * p = pileOfXXs.get(i);
  XXRent * pRent = &p->rent;
  Cash c = pRent->cash;
  if (c>0)
    enrich_(i, -c);
  meapOfXXBombs.check();
  unlock();
  return c; //TODO: proper accounts
}

void showXX(XXIx i, XX * p) {
  printf("ix=%4d|nick=%x,lastPaidRent=%d,cash=%'ld,bomb=%d,", i.i, p->rent.nick, p->rent.lastPaidRent, p->rent.cash, p->rent.bomb.i);
  showXXBody(i, &p->body);
}

static XXIx alloc_(Cash cash, XX ** pp, bool * pRecycled) {
  updateTocks();
  XX * p;
  XXIx i = pileOfXXs.alloc(&p, pRecycled);
  p->rent.lastPaidRent = tocksNow();
  p->rent.cash = cash;
  XXBombIx iBomb;
  XXBomb * pBomb;
  bombee.i = i.i;
  meapOfXXBombs.forAll(bombeeSafe2);
  meapOfXXBombs.check();
  meapOfXXBombs.insert(&iBomb, &pBomb, i.i); // onNew should do the rest
  meapOfXXBombs.check();
  //printf("In alloc near end\n");
  //show();
  review_(i);
  if (pp) *pp = p;
  return i;
}

static XXIx alloc(Cash cash, XX ** pp, bool * pRecycled) {
  lock();
  XXIx ret = alloc_(cash, pp, pRecycled);
  unlock();
  return ret;
}

void onNewXXBomb(XXBombIx iBomb, Ix hint) {
  //printf("OnNew: iBomb: %d hint: %d\n", iBomb.i, hint);
  XXBomb * pBomb = pileOfXXBombs.get(iBomb);
  pBomb->who = (XXIx){hint};
  XX * p = pileOfXXs.get(pBomb->who);
  p->rent.bomb = iBomb;
  updateXXDeathWithIxAndBombPointer(pBomb->who, pBomb);
  meapOfXXBombs.check();
}

void onMoveXXBomb(XXBomb * pBomb, XXBombIx to) {
  XX * p = pileOfXXs.get(pBomb->who);
  //printf("Moving bomb for bulk %d from %d to %d\n", pBomb->who.i, p->rent.bomb.i, to.i);
  p->rent.bomb = to;
  meapOfXXBombs.check();
}

void showXXBomb(XXBombIx i, XXBomb * p) {
  printf("tocks=%d,who=%d\n", p->tocks, p->who.i);
}

XXHotel hotelOfXXs = {open, alloc, get, enrich, chargeIfCan, review, rob, kill, count, close, show, showXX};

