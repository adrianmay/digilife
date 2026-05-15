#include <string.h>
#include <pthread.h>
#include "misc/h.h"
#include "XX_raffle/h.h"
#include "XX_pile/2.h"
#include "XX_hotel/h.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static void lock() { pthread_mutex_lock(&mutex); } 
static void unlock() { pthread_mutex_unlock(&mutex); } 

static XXIx left  (XXIx i) {return ( XXIx ){ 2*i.i + 1 };}
static XXIx right (XXIx i) {return ( XXIx ){ 2*i.i + 2 };}
static XXIx parent(XXIx i) {return ( XXIx ){ (i.i-1)/2 };}
static bool isChild(XXIx i) { return i.i>0; }
static bool isChildRightChild(XXIx i) {return i.i%2==0; } //Whole approach could have fewer ifs

//openXXRaffle() {openXXHotel();}

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

static XXIx enter(Cash cash, XXIx iDonor, Weight w, XXTicket * pTicket) {
  lock();
  XX * p;
  bool recycled;
  XXIx i = hotelOfXXs.alloc(cash, iDonor, &p, &recycled);
  memcpy(&p->body.ticket, pTicket, sizeof(XXTicket));
  XXRafle * pR = &p->body.raffle;
  if (!recycled)  pR->l = pR->r = 0; 
  pR->s = w;
  propagateWeightUp(i, w);
  unlock();
  return i;
}

static Cash cancel(XXIx i) {
  lock();
  XX * pB = pileOfXXs.get(i);
  XXRafle * pR = &pB->body.raffle;
  Weight w = pR->s;
  pR->s = 0;
  //When drawing, we'll treat a free slot like a proper node with a self-weight of zero and descend through it.
  propagateWeightUp(i, -w);
  Cash c = hotelOfXXs.rob(i); //Take all money so it soon gets freed...
  unlock();
  return c;
}

// Assumes there are tickets. Look out of onXXsExtinct
static Cash drawBelow(XXIx i, Weight w, XXTicket * pTicket) {
  XX * pB = pileOfXXs.get(i);
  XXRafle * pR = &pB->body.raffle;
  if (pR->l > 0 && w < pR->l)
    return drawBelow(left(i), w, pTicket);
  w -= pR->l;
  if (w < pR->s) {
    memcpy(pTicket, &pB->body.ticket, sizeof(XXTicket));
    hotelOfXXs.rob(i);
    return 0; //TODO: cash
  } 
  if (pR->r == 0) exit(10); //ShouldnT happen
  w -= pR->s;
  return drawBelow(right(i), w, pTicket);
}

static Cash drawAssumeNotEmpty(XXTicket * pTicket) {
  XXIx i0 = (XXIx){0};
  Weight tw = totWeight(i0);
  uint64_t w = randIntBelow(tw);
  return drawBelow(i0, w, pTicket);
}

static bool draw(XXTicket * pTicket, Cash * pCash) {
  bool ret;
  lock();
  if (hotelOfXXs.count() == 0) 
    ret = false;
  else {
    *pCash = drawAssumeNotEmpty(pTicket);
    ret = true;
  }
  unlock();
  return ret;
}

static bool open(Cash cash, XXIx * pI0) { 
  lock();
  bool ret = hotelOfXXs.open(cash); 
  unlock();
  return ret;
}
static void close(FATE f) { 
  lock();
  hotelOfXXs.close(f); 
  unlock();
}

static void show(void) {
  hotelOfXXs.show();
}

XXRaffle raffleOfXXs = { open, enter, cancel, draw, close, show };

void showXXBody(XXBody * p) {
  printf("l=%'ld,s=%'ld,r=%'ld,", p->raffle.l, p->raffle.s, p->raffle.r);
  showXXTicket(&p->ticket);
}

