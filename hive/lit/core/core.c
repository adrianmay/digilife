// core/core.c
#include <string.h>
#include "h.h"
#include "structs.h"


// atomic_int numCores = 0; // Can read with int x = atomic_load(&numWorkersRunning);
//                                   //
// Core cores[MAX_WORKER_THREADS];
// 
// CoreHandle newCore() {
//   int ch = atomic_fetch_add(&numCores, 1);
//   memset(&cores[ch],0,sizeof(Core));
//   cores[ch].handle = ch;
//   return ch;
// }
// 
// Alarm * coreAlarm   (CoreHandle ch) { return &cores[ch].alarm;     }
// void forceYield     (CoreHandle ch) { cores[ch].forceYield = true; }
// 
// void runMob(Mob * p) {
//   for (int x=0, a=0; a < p->body.effort; a++) 
//     x+=a;
// }


