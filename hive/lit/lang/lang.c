// lang/lang.c
#include "h.h"
#include "structs.h"

void runMob(Mob * p) {
  for (int x=0, a=0; a < p->body.effort; a++) 
    x+=a;
}

atomic_int numCores = 0; // Can read with int x = atomic_load(&numWorkersRunning);
                                  //
Core cores[MAX_WORKER_THREADS];

CoreHandle newCore() {
  int ch = atomic_fetch_add(&numCores, 1);
  cores[ch].handle = ch;
  cores[ch].quitting = false;
  return ch;
}

void delCore(CoreHandle ch) {
  atomic_fetch_sub(&numCores, 1);
}

bool quitting(CoreHandle ch) { return cores[ch].quitting; }
