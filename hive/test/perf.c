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

#define LOTS 400000000ull

uint64_t burn(uint64_t l, bool * quit) { 
  uint64_t x; 
  *quit = false;
  for (x=0; x<l && !(*quit); x++); 
  return x; 
}

bool burnAndRead(PerfHandleS phs, uint64_t toBurn, Cycles cycles, bool * pQ) {
  *pQ=false;
  Cycles before = readThreadCycles(phs);
  burn(LOTS*toBurn, pQ);
  Cycles after = readThreadCycles(phs);
  Cycles used = after-before;
  printf("burnAndRead: fh=%d, want=%'ld, got=%'ld\n", phs, cycles, used);
  assertLongApprox(used, cycles);
  return true;
}

bool quits[10] = {false};

static void handler(PerfHandleC phc) {
  //printf("Told to quit %d\n", phc);
  quits[phc] = true;
}

//void * otherThreadInt(void * p) {
//  PerfHandleS phs = initThreadPerf(handler, 2);
//  //arm(phs, LOTS);
//  burnAndRead(phs, 3*LOTS, 3000000000, &quits[2]); 
//  return 0;
//}
//void * otherThread(void * p) {
//  PerfHandleS phs = initThreadPerf(handler, 2);
//  burnAndRead(phs, 3*LOTS, 3000000000, &quits[2]); 
//  return 0;
//}

bool checkUsed() {
  //background(otherThread); // Not included in proc counter
  //PerfHandleS phsProc = initProcPerf(handler, 0);
  //arm(phsProc, LOTS);
  PerfHandleS phs = initThread(handler, 1);
  //background(otherThreadInt);
  setAlarm(phs, LOTS);
  burnAndRead(phs, 4*LOTS, 4000000000, &quits[1]);
  Cycles now = readThreadCycles(phs);
  assertLongApprox(now, 7000000000); // Cos 4 is total in this thread.
  return true;
}
 
bool checkMainProc() {
  initPerf();
  bool q;
  burn(2, &q);
  readProcessCycles();
  return true;
}

bool checkMainThread() {
  PerfHandleS phs = initThread(handler, 1);
  burnAndRead(phs, 1, 1000000000ull, &quits[1]);
  return true;
}

bool checkMainThreadInt() {
  PerfHandleS phs = initThread(handler, 1);
  setAlarm(phs, 1000000000ull);
  burnAndRead(phs, 3, 1000000000ull, &quits[1]);
  setAlarm(phs, 10000000000ull);
  burnAndRead(phs, 50, 10000000000ull, &quits[1]);
  setAlarm(phs, 20000000000ull);
  burnAndRead(phs, 500, 20000000000ull, &quits[1]);
  return true;
}

static void *worker_main(void *arg)
{
  uint64_t * sleeps = (uint64_t *) arg;
  PerfHandleS phs = initThread(handler, sleeps[0]);

  for (int s=0;sleeps[s]!=0;s++) {
    Cycles cycles = 1000000000*sleeps[s];
    setAlarm(phs, cycles);
    burnAndRead(phs, 50000, cycles, &quits[sleeps[0]]);
  }

  return NULL;
}

bool checkManyThreads() {
  uint64_t sleeps[2][5] = { {1, 4, 1, 0, 0}
                          , {2, 2, 2, 3, 0} };
  pthread_t a, b;
  pthread_create(&a, NULL, worker_main, sleeps[0]);
  pthread_create(&b, NULL, worker_main, sleeps[1]);
  pthread_join(a, NULL);
  pthread_join(b, NULL);
  return true;
}

bool perf(void) {
  return 
    checkHarness(exp1) && 
    checkMainProc() && 
    checkMainThread() && 
    checkMainThreadInt() && 
    checkManyThreads() && 
    printf("Proc cycles total: %'ld\n", readProcessCycles()); //49
    true ;
}



