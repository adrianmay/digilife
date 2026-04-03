#define _POSIX_C_SOURCE 199309L
#include "rent.h"

#define SEC_TO_NS(sec) ((sec)*1000000000)

typedef Tocks (*KILLER)();

typedef struct {
  Pilehead * ph; // The meap pile
  void (*onKill)(void * victim);
} RentContext;

Tocks    wrapSubtractTocks(Tocks a, Tocks b) { return wrapSubtract32(a, b); }
Tocks    wrapAddTocks     (Tocks a, Tocks b) { return wrapAdd32     (a, b); }

void updateTocks() {
  Nanosecs now = ageOfProcess();
  Nanosecs sleptFor = now - vg.tocksReviewedAt;
  Nanosecs toBill = sleptFor + pg->nsNotTocked;
  lldiv_t qr = lldiv(toBill, pg->nsPerTock);
  pg->lastKnownTock = qr.quot;
  pg->nsNotTocked = qr.rem;
} 


void reviewTockDuration() { // Advanced stuff to keep the memory usage slightly below max
}


// This is the main sleepy rent killer and tock tracker.
// It doesn't care if the sleep times out or is interrupted.
void rentCollector(KILLER killer) {
  while (vg.shouldRun) {
    updateTocks();  
    Tocks wakeat1 = killer(); 
    // Race: If right now, new low is reached by e.g. lowest making payment, then will oversleep.
    vg.rentSleeperTid = sleepNs(pg->nsPerTock * wrapSubtractTocks(wakeat1,  pg->lastKnownTock));
    // But if now, it will have woken the sleeper, so OK
    Tocks wakeat2 = killer(); // Solution 
    if (wakeat2 < wakeat1) wake(vg.rentSleeperTid);
    reviewTockDuration();
    wait(vg.rentSleeperTid);
  }
}

