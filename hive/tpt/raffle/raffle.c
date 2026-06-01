#include <string.h>
#include <pthread.h>
#include "misc/h.h"
#include "XX_raffle/h.h"
#include "XX_pile/2.h"
#include "XX_hotel/h.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
static void lock() { pthread_mutex_lock(&mutex); }
static void unlock() { pthread_mutex_unlock(&mutex); }

static XXIx left  (XXIx i) {return ( XXIx ){ 2*i.i + 1 };}
static XXIx right (XXIx i) {return ( XXIx ){ 2*i.i + 2 };}
static XXIx parent(XXIx i) {return ( XXIx ){ (i.i-1)/2 };}
static bool isChild(XXIx i) { return i.i>0; }
static bool isChildRightChild(XXIx i) {return i.i%2==0; } //Whole approach could have fewer ifs

//openXXRaffle() {openXXHotel();}

bool gottaQuitXX = false;

static void show(void) {
  hotelOfXXs.show();
}

static void kill(void) {
  hotelOfXXs.kill();
}
static Weight totWeight(XXIx i) {
  XX * pB = hotelOfXXs.get(i);
  XXRafle * pR = &pB->body.raffle;
  return pR->s + pR->l + pR->r;
}

static void propagateWeightUp(XXIx i, Weight w) {
  // i's weight is already correct, this adjusts the ancestors
  if (!isChild(i)) return;
  XXIx iP = parent(i);
  if (isChildRightChild(i))
    pileOfXXs.get(iP)->body.raffle.r += w;
  else
    pileOfXXs.get(iP)->body.raffle.l += w;
  propagateWeightUp(iP, w);
}

static bool empty() {
  if (hotelOfXXs.count() == 0)
    return true;
  XXIx i0 = (XXIx){0};
  Weight tw = totWeight(i0);
  if (tw==0)
    return true;
  return false;
}

static XXIx enter(Cash cash, Weight w, XXTicket * pTicket) {
  lock();
  bool wasEmpty = empty();
  XX * p;
  bool recycled;
  XXIx i = hotelOfXXs.alloc(cash, &p, &recycled);
  memcpy(&p->body.ticket, pTicket, sizeof(XXTicket));
  XXRafle * pR = &p->body.raffle;
  if (!recycled)  pR->l = pR->r = 0;
  pR->s = w;
  propagateWeightUp(i, w);
  if (wasEmpty) pthread_cond_signal(&cond);
  unlock();
  return i;
}

static Cash cancel_(XXIx i) {
  XX * pB = pileOfXXs.get(i);
  XXRafle * pR = &pB->body.raffle;
  Weight w = pR->s;
  pR->s = 0;
  //When drawing, we'll treat a free slot like a proper node with a self-weight of zero and descend through it.
  propagateWeightUp(i, -w);
  Cash c = hotelOfXXs.rob(i); //Take all money so it soon gets freed...
  return c;
}

static Cash cancel(XXIx i) {
  lock();
  Cash c = cancel_(i);
  unlock();
  return c;
}

static void panicExit(Weight w, XXIx i) {
  XX * pB = pileOfXXs.get(i);
  XXRafle * pR = &pB->body.raffle;
  printf("In panic: w=%ld, i=%d, l=%ld, s=%ld, r=%ld\n", w, i.i, pR->l, pR->s, pR->r );
  if (i.i==0) {  return; }
  panicExit(w, parent(i));
}
// Assumes there are tickets. Look out of onXXsExtinct
static Cash drawBelow(XXIx i, Weight w, XXTicket * pTicket) {
  XX * pB = pileOfXXs.get(i);
  XXRafle * pR = &pB->body.raffle;
  if (pR->l > 0 && w < pR->l)
    return drawBelow(left(i), w, pTicket);
  w -= pR->l;
  if (w < pR->s) {
    Cash c = pB->rent.cash;
    memcpy(pTicket, &pB->body.ticket, sizeof(XXTicket));
    cancel_(i);
    //printf("Returning cash=%ld from drawBelow\n", c);
    return c;
  }
  w -= pR->s;
  if (pR->r == 0) {
    printf("Bailing from drawBelow\n");
    panicExit(w, i);
    exit(10);
  }
  return drawBelow(right(i), w, pTicket);
}

static Cash drawAssumeNotEmpty(XXTicket * pTicket) {
  XXIx i0 = (XXIx){0};
  Weight tw = totWeight(i0);
  uint64_t w = randIntBelow(tw);
  //printf("Rolled w=%ld of %ld\n", w, tw);
  return drawBelow(i0, w, pTicket);
}

// If this returns false we're closing down the program
static bool draw(XXTicket * pTicket, Cash * pCash) {
  lock();
  if (gottaQuitXX)    { gottaQuitXX=false; unlock(); return false; }
  else if (empty()) {
    pthread_cond_wait(&cond, &mutex);
  }
  if (gottaQuitXX)    { gottaQuitXX=false; unlock(); return false; }
  // Can't be empty
  *pCash = drawAssumeNotEmpty(pTicket);
  unlock();
  return true;
}

static bool open() {
  lock();
  bool ret = hotelOfXXs.open();
  unlock();
  return ret;
}
static void close(FATE f) {
  lock();
  hotelOfXXs.close(f);
  unlock();
}

static bool check_(XXIx i) {
  XXRafle * pP = &pileOfXXs.get(      i )->body.raffle;
  XXRafle * pL = &pileOfXXs.get(left (i))->body.raffle;
  XXRafle * pR = &pileOfXXs.get(right(i))->body.raffle;
  return (pP->l == 0 || (pP->l == pL->l + pL->s + pL->r && check_(left (i))))
      && (pP->r == 0 || (pP->r == pR->l + pR->s + pR->r && check_(right(i))));
}

static bool check() { return check_((XXIx) {0}); }

static void quitNow() {
  gottaQuitXX = true;
  pthread_cond_signal(&cond);
}

XXRaffle raffleOfXXs = { open, enter, cancel, empty, draw, close, show, check, kill, quitNow };

void showXXBody(XXBody * p) {
  printf("l=%'ld,s=%'ld,r=%'ld,", p->raffle.l, p->raffle.s, p->raffle.r);
  showXXTicket(&p->ticket);
}

