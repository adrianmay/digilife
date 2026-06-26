#include "h.h"
#include "Link_pile/api.h"
#include "bit/Link.h"

bool recycledSlot;

bool recycledity(void) {
  bool vir1 = pileOfLinks.open(); //Assume it doesn't exist
  assertInt(vir1,true);
  pileOfLinks.alloc(0, &recycledSlot);
  pileOfLinks.close(Nowt); //Don't delete the pile
  bool vir2 = pileOfLinks.open();
  assertInt(vir2,false);
  pileOfLinks.close(Hide); //Delete it for next time
  return true;
}

int sumLinks(LinkIx i0) {
  Link * pT;
  int total=0;
  for (LinkIx i=i0;pileOfLinks.indexValid(i);i=pT->next) { pT = pileOfLinks.get(i); total += pT->x;}
  return total;
}

bool sumItems(LinkIx * i0) {
  bool vir1 = pileOfLinks.open(); //Assume it doesn't exist
  assertInt(vir1,true);
  LinkIx i; Link * pT;
  *i0 = i = pileOfLinks.alloc(&pT, &recycledSlot); // Must be index zero
  pT->x = 0;
  Link * pNew;
  for (int a=0;a<40;a++) {
    pT->next=pileOfLinks.alloc(&pNew, &recycledSlot);
    pNew->next = badLinkIx;
    pNew->x = 10*a;
    pT=pNew;
  }
  int total = sumLinks(*i0);
  assertInt(total,7800);
  return true;
}

LinkIx nextLink(LinkIx i) { return pileOfLinks.get(i)->next; }

bool freeing(LinkIx i0) {
  int count = pileOfLinks.count();
  assertInt(count,41);
  LinkIx i1 = nextLink(i0); //1
  LinkIx i2 = nextLink(i1); //2
  LinkIx i3 = nextLink(i2);
  LinkIx i4 = nextLink(i3);
  LinkIx i5 = nextLink(i4);
  pileOfLinks.free(i2);
  pileOfLinks.free(i4);
  pileOfLinks.free(i3);
  pileOfLinks.get(i1)->next = i5; // 6
  int total = sumLinks(i0);
  assertInt(total,7740);
  count = pileOfLinks.count();
  assertInt(count,38);
  return true;
}

bool reallocing(void) { //Free list = 2,3,4
  LinkIx i = pileOfLinks.alloc(0, &recycledSlot);
  assertInt(i.i,2);
  i = pileOfLinks.alloc(0, &recycledSlot);
  assertInt(i.i,4);
  i = pileOfLinks.alloc(0, &recycledSlot);
  assertInt(i.i,3);
  i = pileOfLinks.alloc(0, &recycledSlot);
  assertInt(i.i,41); // Keeping the 5 and 6
  int count = pileOfLinks.count();
  assertInt(count,42);
  return true;
}

void * incrementBy(Link * p, void * u) {
  int * pi = ((int*)u);
  p->x += *pi;
  (*pi)*=2;
  return 0;
}

// bool with(LinkIx i0) {
//   int total = sumLinks(i0);
//   assertInt(total,7740);
//   int i = 3;
//   pileOfLinks.with(i0, incrementBy, ((void*)&i));
//   total = sumLinks(i0);
//   assertInt(total,7743);
//   assertInt(i,6);
//   return true;
// }

bool showIt(void) {
  pileOfLinks.show(false);
  return false;
}

void act(LinkIx i, Link * p) {
  printf("Foo");
}

bool forall() {
  pileOfLinks.forAll(false, act);
  return true;
}

bool testLinkPile(void) {
  LinkIx i0;
  return true
         && recycledity()  // deletes the pilee
         && (sumItems(&i0), true) // leaves it open
         && freeing(i0) // ditto
         //&& showIt()
         && reallocing()
         //&& showIt()
         //&& with(i0)
         && forall()
;}

void cleanupLinkPile(void) { pileOfLinks.close(Hide); }
bool pile(void) { return bkt("pile", nowt,testLinkPile,cleanupLinkPile); }

void showLink(Link * pLink) {
  printf("x=%d,next=%d\n", pLink->x, pLink->next.i);
}
