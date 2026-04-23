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

typedef Generation uint64_t;
static pthread_mutex_t mutex;
static Generation generation=0;
static pthread_t killerThread;

static void open() {
  pthread_mutex_init(&mutex, 0);
  pileOfXXBulks.open();
  pileOfXXBombs.open();

}

static void close(FATE fate) {
  pileOfXXBulks.close(fate);
  pileOfXXBombs.close(fate);
}

static void updateXXDeath(XXBulk* pBulk, XXBomb * pBomb) {
  Cash cash = pBulk->rent.cash;
  Tocks ttl = cash/tockPrice();
  Tocks death = tocksNow() + ttl;
  pthread_mutex_lock(&mutex);
  pBomb->when = death;
  if (meapOfXXBombs.review(pBulk->rent.bomb))
    pthread_kill(killerThread, SIGUSR1);
  pthread_mutex_unlock(&mutex);
}

static void updateXXDeathWithBulkIndex(XXBulkIndex iBulk) {
  XXBulk * pBulk = pileOfXXBulks.get(iBulk);
  XXBombIndex iBomb = pBulk->rent.bomb;
  XXBomb * pBomb = pileOfXXBombs.get(iBomb);
  updateXXDeath(pBulk, pBomb);
}
  
static void updateXXDeathWithBulkIndexAndBombPointer(XXBulkIndex iBulk, XXBomb * pBomb) {
  XXBulk * pBulk = pileOfXXBulks.get(iBulk);
  updateXXDeath(pBulk, pBomb);
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

static void timedwait(Tocks deadline) {
  printf("timedwait: deadline: %'d\n", deadline);
  Nanosecs ns = nsAtTock(deadline);
  printf("timedwait: ns %'ld\n", ns);
  struct timespec ts;
  lldiv_t qr = lldiv(ns, 1000000000);
  ts.tv_sec = qr.quot;
  ts.tv_nsec= qr.rem;
}

static void * killer(void * p) {
  Tocks deadline;
  killerThread = pthread_self();
  while (true) {
    pthread_mutex_lock(&mutex);
    Generation my_gen = generation;
    if (meapOfXXBombs.size()==0) { pthread_mutex_unlock(&mutex); return 0; } //Extinct
    updateTocks(); // Maybe overkill if the lock below is always fast
    XXBombIndex iBomb = (XXBombIndex) {0};
    XXBomb * pBomb = pileOfXXBombs.get(iBomb);
    Tocks now = tocksNow();
    deadline = pBomb->when;
    if (deadline <= now) {
      pthread_mutex_unlock(&mutex);
      printf("Freeing stuff: iBomb: %d, who: %d\n", iBomb.i, pBomb->who.i);
      pileOfXXBulks.free(pBomb->who);
      pthread_mutex_lock(&mutex);
      meapOfXXBombs.remove(iBomb);
      continue;
    }
    timedwait(deadline);
  }
  pthread_mutex_unlock(&mutex);
  return 0;
}

XXHotel hotelOfXXs = {open, alloc, review, killer, close};

Score getXXBombScore(XXBomb * pBomb) { 
  return pBomb->when; 
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

