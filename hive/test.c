#include <stdio.h>

#include "pile.h"
#include <assert.h>


MAKEPILE1(Thing);

typedef struct __attribute__((aligned(64))) { 
  int x;
  ThingIndex next;
} Thing;


Thing prototypeThing = { 0, (ThingIndex) {BAD_INDEX} };
MAKEPILE2(Thing, GIGA);

int main() { 
  printf("Running pile tests\n"); 
  bool vir1 = openThingPile(); //Assume it doesn't exist
  assert(vir1);
  ThingIndex i = allocThing();
  closeThingPile(false); //Don't delete the pile
  bool vir2 = openThingPile();
  assert(!vir2);
  closeThingPile(true); //Delete it for next time
                        
  bool vir3 = openThingPile(); //Assume it doesn't exist
  assert(vir3);
  ThingIndex zi;
  zi = i = allocThing(); // Must be index zero
  Thing * pT = getThing(i);
  for (int a=0;a<10;a++) {
    pT->next=allocThing();
    pT = getThing(pT->next);
    pT->x = 10*a;
  }
  int total=0;
  for (i=zi;validThingIndex(i);i=pT->next) { pT = getThing(i); total += pT->x;}
  assert(total==450);
  closeThingPile(false); //Delete it for next time
  hideThingPile();                      
  
  return 0; 
}
