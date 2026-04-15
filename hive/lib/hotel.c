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
  vg.tocksReviewedAt = now;
  lldiv_t qr = lldiv(toBill, pg->nsPerTock);
  pg->lastKnownTock += qr.quot;
  pg->nsNotTocked = qr.rem;
} 

void reviewTockDuration() { // Advanced stuff to keep the memory usage slightly below max
}

