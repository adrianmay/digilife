#include "global.h"

void initVirginPersistentGlobals() {
  pg->lastKnownTock = 0;
  pg->nsNotTocked = 0;
  pg->nsPerTock = 1000;
  pg->groatsPerTock = 1;
}

void initVolatileGlobals() {
  vg.tocksReviewedAt = 0;
  vg.shouldRun = true;
}

