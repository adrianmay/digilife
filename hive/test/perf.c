/////////////////////////////////////////////
////// test/perf.c

#define _GNU_SOURCE
#include <string.h>
#include "types.h"
#include "test.h"
#include "assert.h"
#include "misc/h.h"
#include "perf/h.h"

#define LOTS 400000000ull

uint64_t burn(uint64_t l, bool * quit) {
  uint64_t x;
  *quit = false;
  for (x=0; x<l && !(*quit); x++);
  return x;
}

bool burnAndRead(Alarm * pA, uint64_t toBurn, Cycles cycles, bool * pQ) {
  *pQ=false;
  Cycles before = readAlarmCycles(pA);
  burn(LOTS*toBurn, pQ);
  Cycles after = readAlarmCycles(pA);
  Cycles used = after-before;
  printf("burnAndRead: fh=%d, want=%'ld, got=%'ld\n", pA->t.fd, cycles, used);
  assertLongApprox(used, cycles);
  return true;
}

#define MR 5

Cycles res1[MR];

void graft(uint64_t w) { int x; for (int a=0;a<w;a++) x+=x; }

Cycles readTimer(Timer t) {
  return readThreadCycles(t);
}


bool checkProcessTimer() {
  Cycles prev = readProcessCycles();
  for (int a=0;a<MR;a++) {
    graft(1000000000);
    Cycles c = readProcessCycles();
    res1[a] = c-prev;
    prev = c;
  }
  for (int a=0;a<MR;a++)
    printf("checkProcessTimer: diff=%'ld\n", res1[a]);
  return true;
}

bool checkThreadTimer() {
  Timer t = initThreadTimer();
  Cycles prev = readTimer(t);
  for (int a=0;a<MR;a++) {
    graft(1000000000);
    Cycles c = readTimer(t);
    res1[a] = c-prev;
    prev = c;
  }
  for (int a=0;a<MR;a++)
    printf("checkThreadTimer: diff=%'ld\n", res1[a]);
  return true;
}

typedef struct {
  uint64_t sleeps[5];
  bool byeee;
} Wkr;

Wkr wkrs[2] =
  { { {1, 4, 1, 0, 0}, false }
  , { {2, 2, 2, 3, 0}, false }
  };

static void handler(PerfHandleC phc) {
  Wkr * pW = (Wkr *) phc;
  //printf("Told to quit %d\n", phc);
  pW->byeee = true;
}

Alarm a;
int phc = 0;

bool checkThreadAlarm() {
  Wkr * pW = &wkrs[0];
  pW->byeee = false;
  setAlarm(&a, 10000000000);
  for (unsigned long long i=0;!pW->byeee;i++)
    if (0==i%200000000)
      printf("In checkThreadAlarm: %'ld\n", readAlarmCycles(&a));
  return true;
}

static void *worker_main(void *arg)
{
  Alarm a;
  Wkr * pW = (Wkr *) arg;
  initThreadAlarm(&a, handler, pW);

  for (int s=0;pW->sleeps[s]!=0;s++) {
    Cycles cycles = 1000000000*pW->sleeps[s];
    setAlarm(&a, cycles);
    burnAndRead(&a, 50000, cycles, &pW->byeee);
  }

  return NULL;
}

bool checkManyThreads() {
  uint64_t sleeps[2][5] = { {1, 4, 1, 0, 0}
                          , {2, 2, 2, 3, 0} };
  pthread_t a, b;
  pthread_create(&a, 0, worker_main, sleeps[0]);
  pthread_create(&b, 0, worker_main, sleeps[1]);
  pthread_join(a, 0);
  pthread_join(b, 0);
  return true;
}

bool perf(void) {
  initPerf();
  return
    checkProcessTimer() &&
    checkThreadTimer() &&
    (initThreadAlarm(&a, handler, &wkrs[0]), true) &&
    checkThreadAlarm() &&
    checkManyThreads() &&
    (printf("Proc cycles total: %'ld\n", readProcessCycles()), true);
}



