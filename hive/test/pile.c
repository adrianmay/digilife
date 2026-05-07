#include "test.h"
#include "Link_pile/2.h"

bool virginity() {
  bool vir1 = pileOfLinks.open(); //Assume it doesn't exist
  assertInt(vir1,true);
  pileOfLinks.alloc(0);
  pileOfLinks.close(NOWT); //Don't delete the pile
  bool vir2 = pileOfLinks.open();
  assertInt(vir2,false);
  pileOfLinks.close(DELETE); //Delete it for next time
  return true;
}

int sumLinks(LinkIndex i0) {
  Link * pT;
  int total=0;
  for (LinkIndex i=i0;pileOfLinks.indexValid(i);i=pT->next) { pT = pileOfLinks.get(i); total += pT->x;}
  return total;
}

bool sumItems(LinkIndex * i0) {
  bool vir1 = pileOfLinks.open(); //Assume it doesn't exist
  assertInt(vir1,true);
  LinkIndex i; Link * pT;
  *i0 = i = pileOfLinks.alloc(&pT); // Must be index zero
  pT->x = 0; 
  Link * pNew;
  for (int a=0;a<40;a++) {
    pT->next=pileOfLinks.alloc(&pNew);
    pNew->next = badLinkIndex;                       
    pNew->x = 10*a;
    pT=pNew;
  }
  int total = sumLinks(*i0);
  assertInt(total,7800);
  return true;
}

LinkIndex nextLink(LinkIndex i) { return pileOfLinks.get(i)->next; }

bool freeing(LinkIndex i0) {
  int count = pileOfLinks.count();
  assertInt(count,41);
  LinkIndex i1 = nextLink(i0); //1
  LinkIndex i2 = nextLink(i1); //2
  LinkIndex i3 = nextLink(i2);
  LinkIndex i4 = nextLink(i3);
  LinkIndex i5 = nextLink(i4);
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

bool reallocing() { //Free list = 2,3,4
  LinkIndex i = pileOfLinks.alloc(0);
  assertInt(i.i,2);
  i = pileOfLinks.alloc(0);
  assertInt(i.i,4);
  i = pileOfLinks.alloc(0);
  assertInt(i.i,3);
  i = pileOfLinks.alloc(0);
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

bool with(LinkIndex i0) {
  int total = sumLinks(i0);
  assertInt(total,7740);
  int i = 3;
  pileOfLinks.with(i0, incrementBy, ((void*)&i));
  total = sumLinks(i0);
  assertInt(total,7743);
  assertInt(i,6);
  return true;  
}

bool showIt() {
  pileOfLinks.show();
  return false;
}
bool testLinkPile() {
  LinkIndex i0;
  return true
         && virginity()  // deletes the pilee
         && (sumItems(&i0), true) // leaves it open
         && freeing(i0) // ditto
         //&& showIt()
         && reallocing()
         //&& showIt()
         && with(i0)
;}

void cleanupLinkPile() { pileOfLinks.close(DELETE); }
bool pile() { return bkt("pile", nowt,testLinkPile,cleanupLinkPile); }

void showLink(Link * pLink) {
  printf("x=%d,next=%d\n", pLink->x, pLink->next.i);
}
