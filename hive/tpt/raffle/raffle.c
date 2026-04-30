#include "Msg_raffle/h.h"
#include "XXBomb_pile/1.h"
#include "XX_hotel/Bulk.h"
#include "XXBulk_pile/2.h"
#include "XX_hotel/h.h"

//static XXBulkIndex left  (XXBulkIndex i) {return ( XXBulkIndex ){ 2*i.i + 1 };}
//static XXBulkIndex right (XXBulkIndex i) {return ( XXBulkIndex ){ 2*i.i + 2 };}
static XXBulkIndex parent(XXBulkIndex i) {return ( XXBulkIndex ){ (i.i-1)/2 };}
static bool isChild(XXBulkIndex i) { return i.i>0; }
static bool isChildRightChild(XXBulkIndex i) {return i.i%2==0; } //Whole approach could have fewer ifs

void propagateWeightUp(XXBulkIndex i, Weight w) {
  // i's weight is already correct, this adjusts the ancestors
  if (!isChild(i)) return;
  XXBulkIndex iP = parent(i);
  if (isChildRightChild(i))
    pileOfXXBulks.get(iP)->body.raffle.r += w;
  else 
    pileOfXXBulks.get(iP)->body.raffle.l += w; 
  propagateWeightUp(iP, w);
}

XXBulkIndex enter(Cash cash, Weight w, XXTicket * pTicket) {
  XXBulk * pBulk;
  XXBulkIndex iBulk = hotelOfXXs.alloc(cash, &pBulk);
  pBulk->body.raffle.l = 0; // No because if this slot was "freed" earlier there might be children
  pBulk->body.raffle.r = 0; // We can test that by comparing the index with top. (Not freedness in general, but presence of children)
  pBulk->body.raffle.t = w;
  propagateWeightUp(iBulk, w);
  return iBulk;
}

Cash cancel(XXBulkIndex i) {
  XXBulk * pB = pileOfXXBulks.get(i);
  XXRaffle * pR = &pB->body.raffle;
  Weight w = pR->t - pR->l - pR->r;
  pR->t -= w;
  Cash c = pileOfXXBulks.rob(i); //Take all money so it soon gets freed...
  //When drawing, we'll treat a free slot like a proper node with a self-weight of zero and descend through it.
  propagateWeightUp(i, -w);
  return c;
}

Cash draw(XXTicket * pTicket) {
  return 0;
}


