include(pile.m4)
#include "test.m4.h"

IMPL_PILE(Thing, B32, KILO)
Thing prototypeThing = { 0, 0, 0 };

IMPL_MEAP(Thing, B32, KILO)
ThingMeap prototypeThingMeap = { 0, 0 };

MeapScore meapScoreOfThing(ThingMeap * pMeap) {
  return getThing(pMeap->idx)->cash;
}
