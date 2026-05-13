#include "types.h"

typedef struct {
  Cycles tocksReviewedAt; // Exact CPU uptime 
  bool shouldRun;                          
} VolatileGlobals;

typedef struct __attribute__((aligned(KILO))) { 
  int fd; // For globals
  Tocks lastKnownTock; // 
  Cycles cyclesNotTocked; // Tocks were rounded down, such that this much time was not charged
  TockDuration cyclesPerTock; // Easy way to adjust price per ns, while keeping price per tock fixed.
  TockPrice groatsPerTock;                       
} PersistentGlobals;

extern VolatileGlobals vg; 
extern PersistentGlobals * pg; 
