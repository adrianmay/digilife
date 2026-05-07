#include <string.h>
#include "misc/h.h"
#include "XX_raffle/h.h"
#include "XXBulk_pile/2.h"
#include "XX_hotel/h.h"

static XXBulkIndex left  (XXBulkIndex i) {return ( XXBulkIndex ){ 2*i.i + 1 };}
static XXBulkIndex right (XXBulkIndex i) {return ( XXBulkIndex ){ 2*i.i + 2 };}
static XXBulkIndex parent(XXBulkIndex i) {return ( XXBulkIndex ){ (i.i-1)/2 };}
static bool isChild(XXBulkIndex i) { return i.i>0; }
static bool isChildRightChild(XXBulkIndex i) {return i.i%2==0; } //Whole approach could have fewer ifs

//openXXRaffle() {openXXHotel();}

static Weight totWeight(XXBulkIndex i) {
  XXBulk * pB = hotelOfXXs.get(i);
  XXRafle * pR = &pB->body.raffle;
  return pR->s + pR->l + pR->r;
}

static void propagateWeightUp(XXBulkIndex i, Weight w) {
  // i's weight is already correct, this adjusts the ancestors
  if (!isChild(i)) return;
  XXBulkIndex iP = parent(i);
  if (isChildRightChild(i))
    pileOfXXBulks.get(iP)->body.raffle.r += w;
  else 
    pileOfXXBulks.get(iP)->body.raffle.l += w; 
  propagateWeightUp(iP, w);
}

static XXBulkIndex enter(Cash cash, Weight w, XXTicket * pTicket) {
  XXBulk * pBulk;
  XXBulkIndex iBulk = hotelOfXXs.alloc(cash, &pBulk);
  XXRafle * pR = &pBulk->body.raffle;
  pR->s = w;
  pR->l = totWeight(left (iBulk));
  pR->r = totWeight(right(iBulk));
  propagateWeightUp(iBulk, w);
  return iBulk;
}

static Cash cancel(XXBulkIndex i) {
  XXBulk * pB = pileOfXXBulks.get(i);
  XXRafle * pR = &pB->body.raffle;
  Weight w = pR->s;
  pR->s = 0;
  //When drawing, we'll treat a free slot like a proper node with a self-weight of zero and descend through it.
  propagateWeightUp(i, -w);
  Cash c = hotelOfXXs.rob(i); //Take all money so it soon gets freed...
  return c;
}

static Cash drawBelow(XXBulkIndex i, Weight w, XXTicket * pTicket) {
  XXBulk * pB = pileOfXXBulks.get(i);
  XXRafle * pR = &pB->body.raffle;
  if (w < pR->l)
    return drawBelow(left(i), w, pTicket);
  w -= pR->l;
  if (w < pR->s) {
    memcpy(pTicket, &pB->body.ticket, sizeof(XXTicket));
    return 0; //TODO: cash
  } 
  w -= pR->s;
  return drawBelow(right(i), w, pTicket);
}

static Cash draw(XXTicket * pTicket) {
  XXBulkIndex i0 = (XXBulkIndex){0};
  Weight tw = totWeight(i0);
  uint64_t w = randIntBelow(tw);
  return drawBelow(i0, w, pTicket);
}


static bool open() { return hotelOfXXs.open(); }
static void close(FATE f) { hotelOfXXs.close(f); }

XXRaffle raffleOfXXs = { open, enter, cancel, draw, close};

void showXXBody(XXBody * p) {
  printf("l=%'ld,s=%'ld,r=%'ld,", p->raffle.l, p->raffle.s, p->raffle.r);
  showXXTicket(&p->ticket);
}

