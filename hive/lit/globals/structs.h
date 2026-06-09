#include "types.h"

typedef struct {
  Cycles tocksReviewedAt; // Not used in deterministic version.
  bool shouldRun;
} VolatileGlobals;

typedef struct __attribute__((aligned(KILO))) {
  int fd; // For globals
  Tocks lastKnownTock; //
  Cycles cyclesNotTocked; // Tocks were rounded down, such that this much time was not charged
  TockDuration cyclesPerTock; // Easy way to adjust price per ns, while keeping price per tock fixed.
  TockPrice groatsPerTockPerByte;
} PersistentGlobals;

extern VolatileGlobals vg;
extern PersistentGlobals * pg;
