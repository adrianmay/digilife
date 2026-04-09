MAKEPILE1(Thing);

typedef struct __attribute__((aligned(KILO))) { 
  int x;
  ThingIndex next;
} Thing;

MAKEPILE2(Thing, GIGA);

bool virginity() {
  bool vir1 = openThingPile(); //Assume it doesn't exist
  assertInt(vir1,true);
  ThingIndex i = allocThing(0);
  closeThingPile(false); //Don't delete the pile
  bool vir2 = openThingPile();
  assertInt(vir2,false);
  closeThingPile(true); //Delete it for next time
  return true;
}

int sumThings(ThingIndex i0) {
  Thing * pT;
  int total=0;
  for (ThingIndex i=i0;validThingIndex(i);i=pT->next) { pT = getThing(i); total += pT->x;}
  return total;
}

bool sumitems(ThingIndex * i0) {
  bool vir = openThingPile(); //Assume it doesn't exist
  Thing * pT;
  assertInt(vir,true);
  ThingIndex i;
  *i0 = i = allocThing(&pT); // Must be index zero
  pT->x = 0; 
  for (int a=0;a<1000;a++) {
    pT->next=allocThing(&pT);
    pT->next = badThingIndex;                       
    pT->x = 10*a;
  }
  int total = sumThings(*i0);
  assertInt(total,4995000);
  return true;
}

ThingIndex nextThing(ThingIndex i) { return getThing(i)->next; }

bool freeing(ThingIndex i0) {
  int count = countThings();
  assertInt(count,1001);
  ThingIndex i1 = nextThing(i0);
  ThingIndex i2 = nextThing(i1);
  ThingIndex i3 = nextThing(i2);
  ThingIndex i4 = nextThing(i3);
  ThingIndex i5 = nextThing(i4);
  freeThing(i2);
  freeThing(i4);
  freeThing(i3);
  getThing(i1)->next = i5;
  int total = sumThings(i0);
  assertInt(total,4994940);
  count = countThings();
  assertInt(count,998);
  return true;
}

bool reallocing() {
  ThingIndex i = allocThing(0);
  assertInt(i.i,3);
  i = allocThing(0);
  assertInt(i.i,1001);
  i = allocThing(0);
  assertInt(i.i,1002);
  i = allocThing(0);
  assertInt(i.i,1003);
  int count = countThings();
  assertInt(count,1002);
  return true;
}

void * incrementBy(Thing * p, void * u) {
  int * pi = ((int*)u);
  p->x += *pi;
  (*pi)*=2;
  return 0;
}

bool with(ThingIndex i0) {
  int total = sumThings(i0);
  assertInt(total,4994940);
  int i = 3;
  withThing(i0, incrementBy, ((void*)&i));
  total = sumThings(i0);
  assertInt(total,4994943);
  assertInt(i,6);
  
}

bool testThingPile() {
  ThingIndex i0;
  return virginity()
      && (sumitems(&i0), true)
      && freeing(i0)
      && reallocing()
      && with(i0)
      ;
}

void cleanupThingPile() { closeThingPile(2); }
bool pile() { return bkt(nowt,testThingPile,cleanupThingPile); }

