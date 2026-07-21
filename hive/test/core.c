#include <time.h>
#include <string.h>
#include <stddef.h>
#include <stdatomic.h>
#include "h.h"
#include "globals/api.h"
#include "Msg_raffle/ix.h"
#include "Mob_hotel/ix.h"
#include "Msg_raffle/api.h"
#include "Mob_hotel/api.h"
#include "core/api.h"
#include "core/Mob.h"
#include "core/Msg.h"

static bool init(void) { 
  openGlobals(); hotelOfMobs_open(); raffleOfMsgs_open(); 
  //printf("Sizes: mob=%f,msg=%f,tot=%f; Props: mob=%f,msg=%f\n", SIZE_MOB, SIZE_MSG, SIZE_BOTH, MOB_PROP, MSG_PROP);
  return true;
}

static void cleanup(void) { 
  closeGlobals(Delete); 
  raffleOfMsgs_close(Delete); 
  hotelOfMobs_close(Delete); 
}

void * work(void * p) {
//  while(iterations < 100000 && draw())  {
  while(draw())  {
    //if (iterations < 1000 || iterations % 1000 == 0)
    //  printf("Its=%d, Tocks=%d Mobs=%d Msgs=%d\n", iterations, tocksNow(), hotelOfMobs_count(), raffleOfMsgs_count()); 
    atomic_fetch_add(&iterations,1);
  }
  return 0;
}

//#define NUM_THREADS 2
//static pthread_t pids[NUM_THREADS] = {0};

// static bool testForever() {
//   onTestTock = onTockCore;
//   assertInt (hotelOfMobs_bodyat(),  MOB_HEADER_SIZE);
//   assertInt (hotelOfMobs_recBlob(), MOB_GROSS_SIZE);
//   assertInt (hotelOfMobs_bodylen(), MOB_BODY_SIZE);
//   //assertInt (MOB_GROSS_SIZE, MOB_HEADER_SIZE + MOB_CODE_SIZE);
//   seed(50, 1000000, 20'000'000); // Number of mobs, starting cash, spawn threshold
//   atomic_store(&iterations, 0);
//   time_t start = time(NULL);
//   work(0);
//   //for (int64_t a=0;a<NUM_THREADS; a++) pthread_create(pids+a, 0, work, (void*)a);
//   //for (int64_t a=0;a<NUM_THREADS; a++) pthread_join(pids[a], 0);
//   time_t end = time(NULL);
//   hotelOfMobs_show();
//   raffleOfMsgs_show();
//   printf("Took %'ld\n", end-start);
//   return true;
// }

#define CORE_OUT_LEN 100
char out[CORE_OUT_LEN];

Program testProgs[] = {
  _print0 "Foo" _0 _print0 "Bar" _0 _end,
  "",
  "",
  "",
  "",
};

#define NUM_TEST_PROGS (sizeof(testProgs)/sizeof(Program))

char testExpectations[NUM_TEST_PROGS][CORE_OUT_LEN] = { "FooBar", };

double bigAmgis = 1;
double bigNegAmgis = -1;
double bigMu = 1000000000;
double smallMu = 10;

void buildRollTest(int t, double * mu, double * amgis, bool which) {
  char * p = (char*) &testProgs[t];
  *p++ = *_rollCash;
  memcpy(p, (char*)mu, sizeof(double));
  p+=sizeof(double);
  memcpy(p, (char*)amgis, sizeof(double));
  p+=sizeof(double);
  memcpy(p, _print0 "H" _0 _snd _print0 "L" _0 _end, 9);
  memcpy(testExpectations[t], which ? "H" : "L", 2);
}

static bool testCode() {
  buildRollTest(1, &smallMu, &bigAmgis, true);
  buildRollTest(2, &bigMu,   &bigAmgis, false);
  buildRollTest(3, &smallMu, &bigNegAmgis, false);
  buildRollTest(4, &bigMu,   &bigNegAmgis, true);
  Mob mob;
  bool res = true;
  for (int t=0;t<NUM_TEST_PROGS;t++) {
    printf("testCode: #%d\n", t);
    memcpy((char*)mob._.mortal.program, (char*)testProgs[t], sizeof(mob._.mortal.program));
    runInCore(1'000'000, &mob, 0, out, CORE_OUT_LEN);
    if (0!=strcmp(out, testExpectations[t])) {
      printf("testCode #%d Failed: want: '%s', got: '%s'\n", t, testExpectations[t], out);
      res = false;
    }
  }
  return res;
}

bool testCore() {
  return 
    testCode() &&
    //testForever() && 
    true;
}

bool core(void) { return bkt("core", init, testCore, cleanup); }

// Expt 2 result:
// Apply 1:20 murder rate per job. Thresh settles low with 1:16 spawn rate. 43% chance surviving til spawn - not.
// Its=200,000,000, Rent=1,000, thresh=8,115,720; Means: pop=488.25, spawnOdds=16.30214, childCash=1,675,726 msgCash=2,443,428, mobCash=1,466,057, totCash=3,909,484

