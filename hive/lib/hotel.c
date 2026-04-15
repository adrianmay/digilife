#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include "hotel.h"
#include "assert.h"

#define SEC_TO_NS(sec) ((sec)*1000000000)


typedef struct {
  Pilehead * ph; // The meap pile
  void (*onKill)(void * victim);
} RentContext;

Tocks    wrapSubTocksU(Tocks a, Tocks b) { return wrapSub32U(a, b); }
TockDiff wrapSubTocksS(Tocks a, Tocks b) { return wrapSub32U(a, b); }
Tocks    wrapAddTocks (Tocks a, Tocks b) { return wrapAdd32     (a, b); }

void updateTocks() {
  Nanosecs now = ageOfProcess();
  Nanosecs sleptFor = wrapSub64U(now, vg.tocksReviewedAt);
  Nanosecs toBill = sleptFor + pg->nsNotTocked;
  lldiv_t qr = lldiv(toBill, pg->nsPerTock);
  pg->lastKnownTock = qr.quot;
  pg->nsNotTocked = qr.rem;
} 

void reviewTockDuration() { // Advanced stuff to keep the memory usage slightly below max
}

// // This is the main sleepy rent killer and tock tracker.
// // It doesn't care if the sleep times out or is interrupted.
bool rentCollector(KILLER killer) {
  while (vg.shouldRun) {
    updateTocks();  
    Tocks wakeat1 = killer(pg->lastKnownTock); 
    if (wakeat1==0) return true;
    // Race: If right now, new low is reached by e.g. lowest making payment, then will oversleep.
    TockDiff tcks = wrapSubTocksS(wakeat1,  pg->lastKnownTock);
    assertCond(tcks, >=0);
    Nanosecs ns = pg->nsPerTock * tcks;
    vg.rentSleeperTid = sleepNs(ns);
    // But if now, it will have woken the sleeper, so OK
    Tocks wakeat2 = killer(pg->lastKnownTock); // Solution 
    if (wakeat2>0 && wakeat2 < wakeat1) wake(vg.rentSleeperTid);
    reviewTockDuration();
    wait(vg.rentSleeperTid);
  }
  return true;
}

