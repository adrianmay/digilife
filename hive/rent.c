#define _POSIX_C_SOURCE 199309L
#include "rent.h"

#define SEC_TO_NS(sec) ((sec)*1000000000)

Tocks    wrapSubtractTocks(Tocks a, Tocks b) { return wrapSubtract32(a, b); }
Tocks    wrapAddTocks     (Tocks a, Tocks b) { return wrapAdd32     (a, b); }

typedef struct {
  uint32_t thingSize;
  void (*onKill)();
} RentContext;

void updateTocks() {
  Nanosecs now = ageOfProcess();
  Nanosecs sleptFor = now - vg.tocksReviewedAt;
  Nanosecs toBill = sleptFor + pg->nsNotTocked;
  lldiv_t qr = lldiv(toBill, pg->nsPerTock);
  pg->lastKnownTock = qr.quot;
  pg->nsNotTocked = qr.rem;
} 

Tocks killAllExpired() { return 0; }

void reviewTockDuration() {}

void sleepUntilTock(Tocks wakeat) {
  sleepNs(pg->nsPerTock * wrapSubtractTocks(wakeat,  pg->lastKnownTock));
}

void rentCollector(RentContext * ctx) {
  while (vg.shouldRun) {
    updateTocks();  
    Tocks nextKill = killAllExpired();
    reviewTockDuration();
    sleepUntilTock(nextKill+1);
  }
}

//    clock_t ticksUsed() {
//      struct tms t;
//      times(&t);
//      clock_t now = t.tms_utime + t.tms_stime;
//      return now;
//    }
//    
//    uint64_t realTime() {
//      static uint64_t last = 0;
//      struct timeval now_;
//      gettimeofday(&now_, 0);
//      uint64_t now = 1000000 * now_.tv_sec + now_.tv_usec;
//      uint64_t ret = now - last;
//      last = now;
//      return ret;
//    }
//    
//    void setTimer(time_t when) {
//      struct tms t;
//      times(&t);
//      clock_t now = t.tms_utime + t.tms_stime;
//      //struct itimerval i; 
//      //getitimer();
//    }
