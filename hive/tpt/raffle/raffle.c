#include "Msg_raffle/h.h"
#include "XXBomb_pile/1.h"
#include "XX_hotel/Bulk.h"
#include "XXBulk_pile/2.h"
#include "XX_hotel/h.h"

// We free things when they're drawn or cancelled but set their self-weight to zero and expect their l and r subtree totals to be preserved.

static bool isChild(XXBulkIndex i) { return i.i>0; }
static bool isChildRightChild(XXBulkIndex i) {return i.i%2==0; } //Whole approach could have fewer ifs
static XXBulkIndex parent(XXBulkIndex i) {return ( XXBulkIndex ){ (i.i-1)/2 };}
static XXBulkIndex left  (XXBulkIndex i) {return ( XXBulkIndex ){ 2*i.i + 1 };}
static XXBulkIndex right (XXBulkIndex i) {return ( XXBulkIndex ){ 2*i.i + 2 };}

Weight weight(XXBulkIndex i) {
  XXBulk * p = pileOfXXBulks.safeGet(i);
  return p ? p->body.raffle.t : 0;
}

void propagateWeightUp(XXBulkIndex i, Weight w) {
  // i's weight is already correct, this adjusts the ancestors
  if (!isChild(i)) return;
  XXBulkIndex iPar = parent(i);
  XXBulk * pPar = pileOfXXBulks.get(iPar);
  pPar->body.raffle.t += w;
  if (isChildRightChild(i))
    pPar->body.raffle.r += w;
  else 
    pPar->body.raffle.l += w; 
  propagateWeightUp(iP, w);
}

XXBulkIndex enter(Cash cash, Weight w, XXTicket * pTicket) {
  XXBulk * pBulk; bool recycled;
  XXBulkIndex iBulk = hotelOfXXs.alloc(cash, &pBulk, &recycled);
  XXRaffle * pRaf = &pBulk->body.raffle;
  if (!recycled) {
    pRaf->t = w;
    pRaf->l = 0; 
    pRaf->r = 0; 
  } else {
    pRaf->t = pRaf->l + pRaf->r  + w;
  }
  propagateWeightUp(iBulk, w);
  return iBulk;
}

Cash cancel(XXBulkIndex i) {
  XXBulk * pB = pileOfXXBulks.get(i);
  XXRaffle * pR = &pB->body.raffle;
  Weight w = pR->t - pR->l - pR->r;
  pR->t -= w; // Set self-weight to zero
  Cash c = hotelOfXXs.rob(i); //Take all money so it soon gets freed...
  //When drawing, we'll treat a free slot like a proper node with a self-weight of zero and descend through it.
  propagateWeightUp(i, -w);
  return c;
}

Cash draw(XXTicket * pTicket) {
  Weight tot = hotelOfXXs.get((XXBulkIndex){0})
  return 0;
}


