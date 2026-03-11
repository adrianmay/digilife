#include <stdio.h>
#include "test.m4.h"


int makeTestPile() { 
  if (!openPileOfThings(false)) return 1;
  allocThing(0);
  return 0;
}
int virgins() { return 0
  || ( openPileOfThings(false)==true   ? 0 : 2   )
  || ( closePileOfThings(false)        , 0       )              
  || ( makeTestPile(false)                       )
  || ( closePileOfThings(false)        , 0       )
  || ( openPileOfThings(false)==false  ? 0 : 6   )
  || ( closePileOfThings(true)         , 0       )
  ;}

bool makeMortalThing(uint64_t n, uint64_t x) {
  ThingIndex i = allocThing(0);
  Thing * pT = getThing(i);
  pT->name = n;
  pT->cash = 100;
  addToThingMeap(i, x, 0);
  return true;
}


int meaps() { 
  if (!openPileOfThings(false)) return 1;
  if (!openPileOfThingMeaps(false)) return 2;
  makeMortalThing(11, 1000);
  makeMortalThing(1, 2000);
  makeMortalThing(5, 5000);
  closePileOfThings(true);
  closePileOfThingMeaps(true);
  return 0;
  ;}

int main() { return 0
  || ( virgins()                                 )
  || ( meaps()                                 )
  ;}
