#include "test.h"
#include "pile.h"
#include "globals.h"

MAKEGLOBALS

bool testGlobals() {
  bool v = openGlobals();
  assertInt(v, true);
  uint64_t i = pg->lastKnownTock + vg.tocksReviewedAt + pg->nsPerTock;
  assertLong(i, 1000000L);
  vg.tocksReviewedAt = 2;
  pg->lastKnownTock = 1;
  pg->nsPerTock = 3;
  closeGlobals(0);
  v = openGlobals(); // Resets vg
  assertInt(v, false);
  i = pg->lastKnownTock + vg.tocksReviewedAt + pg->nsPerTock;
  assertLong(i, 4l);
  return true;
}

void cleanupGlobals() { closeGlobals(1); }
bool globals() { return bkt(nowt,testGlobals,cleanupGlobals); }

