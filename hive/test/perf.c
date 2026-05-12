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

#define LOTS 397000000ull

uint64_t burn(uint64_t l, bool * quit) { 
  uint64_t x; 
  *quit = false;
  for (x=0; x<l && !(*quit); x++); 
  return x; 
}

bool burnAndRead(PerfHandleS phs, uint64_t toBurn, Cycles cycles, bool * pQ) {
  burn(toBurn, pQ);
  Cycles now = readCyclesNow(phs);
  printf("burnAndRead: fh=%d, %'ld\n", phs, now);
  assertLongApprox(now, cycles);
  return true;
}

bool quits[10] = {false};
bool beenhere[10] = {false};

static void handler(PerfHandleC phc) {
  disarm(phc);
  if (!beenhere[phc]) {
    beenhere[phc]=true;
    printf("Ignoring quit %d\n", phc);
    return;
  }
  printf("Told to quit %d\n", phc);
  quits[phc] = true;
}

void * otherThreadInt(void * p) {
  PerfHandleS phs = initThreadPerf(handler, 2);
  //arm(phs, LOTS);
  burnAndRead(phs, 3*LOTS, 3000000000, &quits[2]); 
  return 0;
}
void * otherThread(void * p) {
  PerfHandleS phs = initThreadPerf(handler, 2);
  burnAndRead(phs, 3*LOTS, 3000000000, &quits[2]); 
  return 0;
}

bool checkUsed() {
  background(otherThread); // Not included in proc counter
  PerfHandleS phsProc = initProcPerf(handler, 0);
  //arm(phsProc, LOTS);
  PerfHandleS phs = initThreadPerf(handler, 1);
  background(otherThreadInt);
  burnAndRead(phs, 2*LOTS, 2000000000, &quits[1]);
  burnAndRead(phs, 2*LOTS, 4000000000, &quits[1]);
  Cycles now = readCyclesNow(phsProc);
  assertLongApprox(now, 7000000000); // Cos 4 is total in this thread.
  return true;
}
 
bool perf(void) {
  return 
    //checkHarness(exp1) && 
    checkUsed() && 
    true ;
}



