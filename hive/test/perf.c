/////////////////////////////////////////////
////// test/perf.c

#define _GNU_SOURCE
#include <string.h>
#include "types.h"
#include "test.h"
#include "assert.h"
#include "misc/h.h"
#include "perf/h.h"

//This can monitor threads irrespective of CPU, but not the whole process.
//So memory killing will have to be done by worker threads before
// an animal tries to allocate memory, or whenever.


#define MX 20
char res[MX][MX]={0};
int cur = 0;


void say(const char * msg) { 
  strncpy(res[cur++], msg, MX); 
}


bool check(char e[20][20], int line) {
  char lineS[20];
  sprintf(lineS, "%d", line);
  for (int a=0; a<MX; a++) 
    assertStringAtLine(e[a], res[a], MX, lineS);
  return true;
}

char exp1[MX][MX]={"Foo", "Bar", {0}};
bool checkHarness() {
  return (                   !check(exp1, __LINE__))
      && (       say("Foo"), !check(exp1, __LINE__))
      && (       say("Bur"), !check(exp1, __LINE__))
      && (cur--, say("Bar"),  check(exp1, __LINE__))
      && (       say("Wop"), !check(exp1, __LINE__))
      ;
} 

uint64_t burn(uint64_t l) { uint64_t x; for (x=0; x<l; x++); return x; }

bool checkThreadUsed() {
  PerfHandleC phc = initThreadPerf(0, 0);
  burn(2*GIGA);
  Cycles now = readThreadCyclesNow(phc);
  assertLongCond(now, >1900000000ull)
  assertLongCond(now, <2100000000ull)
  return true;
}
 
bool perf(void) {
  background(sweat_forever);
  return checkHarness(exp1)
      && checkThreadUsed()
      && (sleepNs(1000000000), true);
    ;
}
