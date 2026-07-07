#include "h.h"
#include "globals/api.h"
#include "globals/structs.h"


bool testGlobals(void) {
  bool v = openGlobals();
  assertInt(v, true);
  uint64_t i = pg->lastKnownTock + vg.tocksReviewedAt + pg->cyclesPerTock;
  assertLong(i, 2000L);
  vg.tocksReviewedAt = 2;
  pg->lastKnownTock = 1;
  pg->cyclesPerTock = 3;
  closeGlobals(0);
  v = openGlobals(); // Resets vg
  assertInt(v, false);
  i = pg->lastKnownTock + vg.tocksReviewedAt + pg->cyclesPerTock;
  assertLong(i, 6l);
  return true;
}

void cleanupGlobals(void) { closeGlobals(1); }
bool globals(void) { return bkt("globals", nowt,testGlobals,cleanupGlobals); }
