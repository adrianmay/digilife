#include "test.h"
#include "ipile/h.h"
#include "Link_pile/1.h"
#include "Link_pile/Link.h"
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
  for (int a=0;a<1000;a++) {
    pT->next=pileOfLinks.alloc(&pT);
    pT->next = badLinkIndex;                       
    pT->x = 10*a;
  }
  int total = sumLinks(*i0);
  assertInt(total,4995000);
  return true;
}

LinkIndex nextLink(LinkIndex i) { return pileOfLinks.get(i)->next; }

bool freeing(LinkIndex i0) {
  int count = pileOfLinks.count();
  assertInt(count,1001);
  LinkIndex i1 = nextLink(i0); //3
  LinkIndex i2 = nextLink(i1); //4
  LinkIndex i3 = nextLink(i2);
  LinkIndex i4 = nextLink(i3);
  LinkIndex i5 = nextLink(i4);
  pileOfLinks.free(i2);
  pileOfLinks.free(i4);
  pileOfLinks.free(i3);
  pileOfLinks.get(i1)->next = i5;
  int total = sumLinks(i0);
  assertInt(total,4994940);
  count = pileOfLinks.count();
  assertInt(count,998);
  return true;
}

bool reallocing() { //Free list = 0, 1, 4, 5, 6
  LinkIndex i = pileOfLinks.alloc(0);
  assertInt(i.i,0);
  i = pileOfLinks.alloc(0);
  assertInt(i.i,1);
  i = pileOfLinks.alloc(0);
  assertInt(i.i,4);
  i = pileOfLinks.alloc(0);
  assertInt(i.i,1003); // Keeping the 5 and 6
  int count = pileOfLinks.count();
  assertInt(count,1002);
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
  assertInt(total,4994940);
  int i = 3;
  pileOfLinks.with(i0, incrementBy, ((void*)&i));
  total = sumLinks(i0);
  assertInt(total,4994943);
  assertInt(i,6);
  return true;  
}

bool testLinkPile() {
  LinkIndex i0;
  return true
         && virginity()  // deletes the pilee
         && (sumItems(&i0), true) // leaves it open
         && freeing(i0) // ditto
         && reallocing()
         && with(i0)
;}

void cleanupLinkPile() { pileOfLinks.close(DELETE); }
bool pile() { return bkt("pile", nowt,testLinkPile,cleanupLinkPile); }

