#include "types.h"

typedef struct {
  Nanosecs tocksReviewedAt; // Exact CPU uptime 
  bool shouldRun;                          
  pthread_t rentSleeperTid;
} VolatileGlobals;

typedef struct __attribute__((aligned(KILO))) { 
  int fd; // For globals
  Tocks lastKnownTock; // 
  Nanosecs nsNotTocked; // Tocks were rounded down, such that this much time was not charged
  TockDuration nsPerTock; // Easy way to adjust price per ns, while keeping price per tock fixed.
  TockPrice groatsPerTock;                       
} PersistentGlobals;

extern VolatileGlobals vg;
extern PersistentGlobals * pg;

void initVirginPersistentGlobals();
void initVolatileGlobals();

