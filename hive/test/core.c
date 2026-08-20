#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wparentheses"

#include <time.h>
#include <string.h>
#include <stddef.h>
#include <stdatomic.h>
#include "h.h"
#include "misc/api.h"
#include "globals/api.h"
#include "Msg_raffle/ix.h"
#include "Mob_hotel/ix.h"
#include "Msg_raffle/api.h"
#include "Mob_hotel/api.h"
#include "core/Mob.h"
#include "core/Msg.h"
#include "core/api.h"

#define BIRTH_CASH 1000'000'000L

void showCore() {
  hotelOfMobs_show();
  raffleOfMsgs_show();
}

static bool init(void) { 
  onTestTock = onTockCore;
  openGlobals(); hotelOfMobs_open(); raffleOfMsgs_open(); 
  printf("Sizes: mob=%f,msg=%f,tot=%f; Props: mob=%f,msg=%f\n", SIZE_MOB, SIZE_MSG, SIZE_BOTH, MOB_PROP, MSG_PROP);
  return true;
}

static void cleanup(void) { 
  closeGlobals(Hide); 
  raffleOfMsgs_close(Hide); 
  hotelOfMobs_close(Hide); 
}

//#define NUM_THREADS 2
//static pthread_t pids[NUM_THREADS] = {0};

#define CORE_OUT_LEN 100
char out[CORE_OUT_LEN];
int outlen = CORE_OUT_LEN;

Program testProgs[] = {
  PRS "Foo" NOP PRS "Bar" NOP END,
  IFF YES PRS "A" NOP ELSIF NO  PRS "B" NOP END END,
  IFF YES PRS "A" NOP ELSIF YES PRS "B" NOP END END,
  IFF NO  PRS "A" NOP ELSIF YES PRS "B" NOP END END,
  IFF NO  PRS "A" NOP ELSIF NO  PRS "B" NOP ELSIF YES  PRS "C" NOP END END,
  IFF YES PRS "A" NOP ELSIF NO  PRS "B" NOP ELSIF YES  PRS "C" NOP END END,
  IFF YES PRS "A" NOP ELSIF NO  PRS "B" NOP END  PRS "Z" NOP END,
  IFF YES PRS "A" NOP ELSIF NO  IFF YES PRS "C" NOP ELSIF NO  PRS "D" NOP END END  PRS "Z" NOP END,
  IFF NO  PRS "A" NOP ELSIF YES IFF YES PRS "C" NOP ELSIF NO  PRS "D" NOP END END  PRS "Z" NOP END,
  IFF NO  PRS "A" NOP ELSIF YES IFF NO  PRS "C" NOP ELSIF YES PRS "D" NOP END END  PRS "Z" NOP END,
  IFF NOT YES PRS "A" NOP ELSIF YES PRS "B" NOP END END,
  IFF NOT NOT YES PRS "A" NOP ELSIF YES PRS "B" NOP END END,
  PRF ZERO END,
  PRF ONE END,
  PRF MUL TWO ADD TWO ONE END,
  PRF MUL TWO INV ADD TWO ONE END,
  PRF ADD TWO NEG ADD TWO ONE END,
  IFF GT ZERO ONE PRS "A" NOP ELSIF YES PRS "B" NOP END END,
  IFF LIKE MUL ONE INV ADD TWO TWO ONE TWO PRS "A" NOP ELSIF YES PRS "B" NOP END END,
};

#define NUM_TEST_PROGS (sizeof(testProgs)/sizeof(Program))

char testExpectations[NUM_TEST_PROGS][CORE_OUT_LEN] = { 
  "FooBar", 
  "A",
  "A",
  "B",
  "C",
  "A",
  "AZ",
  "AZ",
  "CZ",
  "DZ",
  "B",
  "A",
  "0.000000",
  "1.000000",
  "6.000000",
  "0.666667",
  "-1.000000",
  "A",
  "B",
};

float bigAmgis = 1;
float bigNegAmgis = -1;
float bigMu = 1000000000;
float smallMu = 10;

void injectOp(char ** p, uint8_t op)  { *(*p)++ = op; }
void injectOps(char ** p, char * ops, int count)  { memcpy(*p, ops, count); *p += count; }
void injectFloatPair(char ** p, float mu, float amgis)  {
  memcpy(*p, (char*)&mu, sizeof(float));
  (*p)+=sizeof(float);
  memcpy(*p, (char*)&amgis, sizeof(float));
  (*p)+=sizeof(float);
}

// #define YEAORNAY(YEA, NAY) injectOps(&p, _print YEA _nop _elsif _yes _print NAY _nop _end, 8);
// 
// void buildRollTest(int *t, float mu, float amgis, bool which) {
//   char * p = (char*) &testProgs[*t];
//   injectOp(&p, *_rollCash);
//   injectFloatPair(&p, mu, amgis);
//   YEAORNAY("H", "L")
//   injectOp(&p, *_end);
//   memcpy(testExpectations[*t], which ? "H" : "L", 2);
//   (*t)++;
// }
// 
// void buildNestedRollTest(int *t, int a, int b, int c, char e) {
//   char * p = (char*) &testProgs[*t];
//   injectOp(&p, *_roll);
//   injectFloatPair(&p, a, 1000000);
//     injectOp(&p, *_roll);
//     injectFloatPair(&p, b, 1000000);
//     YEAORNAY("A", "B")
//   injectOp(&p, *_snd);
//     injectOp(&p, *_roll);
//     injectFloatPair(&p, c, 1000000);
//     YEAORNAY("C", "D")
//   injectOp(&p, *_end);
//   injectOp(&p, *_end);
//   char ex[2] = {e,0};  
//   memcpy(testExpectations[*t], ex, 2);
//   (*t)++;
// }

static bool testCode() {
//  int t = 1;
//  buildRollTest(&t, smallMu, bigAmgis, true);
//  buildRollTest(&t, bigMu,   bigAmgis, false);
//  buildRollTest(&t, smallMu, bigNegAmgis, false);
//  buildRollTest(&t, bigMu,   bigNegAmgis, true);
//
//  buildNestedRollTest(&t, -1, -1, -1, 'A');
//  buildNestedRollTest(&t, -1,  1, -1, 'B');
//  buildNestedRollTest(&t, -1, -1,  1, 'A');
//  buildNestedRollTest(&t, -1,  1,  1, 'B');
//
//  buildNestedRollTest(&t,  1, -1, -1, 'C');
//  buildNestedRollTest(&t,  1,  1, -1, 'C');
//  buildNestedRollTest(&t,  1, -1,  1, 'D');
//  buildNestedRollTest(&t,  1,  1,  1, 'D');

  Mob mob;
  MobTact tMob = (MobTact){{8}, 0x12345678};
  bool res = true;
  for (int t=0;t<NUM_TEST_PROGS;t++) {
    printf("####### testCode: #%d\n", t);
    memcpy((char*)mob._.mortal.program, (char*)testProgs[t], sizeof(mob._.mortal.program));
    runInCore(1'000'000, tMob, &mob, 0);
    if (0!=strcmp(out, testExpectations[t])) {
      printf("testCode #%d Failed: want: '%s', got: '%s'\n", t, testExpectations[t], out);
      res = false;
    }
  }
  return res;
}


static bool testSpawnAndPost() {
  printf("testSpawnAndPost\n");
  Cash birthCash = BIRTH_CASH + randIntBelow(BIRTH_CASH);
  void stuffProg(Program * pProg) {
  }
  seed(10, birthCash, stuffProg);
  //MobTact tMob = (MobTact){{8}, 0x12345678};
  //Mob mob;
  //mob.phylum = PhyMortal;
  //mob._.mortal.spawnThresh = 123;
  //Program spawner = _spawn0 _post0 _end;
  //memcpy((char*)mob._.mortal.program, spawner, sizeof(mob._.mortal.program));
  // Make one real mob from this imaginary mob
  //runInCore(birthCash, tMob, &mob, 0);
  // Check the populations
  Ix popMobs, popMsgs;
  popMobs = hotelOfMobs_count();
  assertInt(popMobs, 10);
  popMsgs = raffleOfMsgs_count();
  assertInt(popMsgs, 10);
  // Inspect it
  MobTact tMob0 = (MobTact){(MobIx){0},0};
  Mob * pMob; Cash cash;
  hotelOfMobs_grabIx(&tMob0, &pMob, &cash);
  //showMob(iMob0, pMob);
  //assertLong(pMob->_.mortal.spawnThresh, 123L);
  Cash expect = birthCash*MOB_PROP;
  assertLong(cash, expect);
  hotelOfMobs_drop(tMob0.i, cash);
  // Run the one mob in the hotel:
  draw();
  draw();
  draw();
  // Check the populations // raid
  popMobs = hotelOfMobs_count();
  assertInt(popMobs, 13);
  popMsgs = raffleOfMsgs_count();
  assertInt(popMsgs, 13);
  //showMsgTicket((MsgTicketIx){0},0); printf("\n");
  return true;
}

void * work(void * p) {
  while(iterations < 100000000 && draw())  {
//  while(draw())  {
    //if (iterations < 1000 || iterations % 1000 == 0)
    //  printf("Its=%d, Tocks=%d Mobs=%d Msgs=%d\n", iterations, tocksNow(), hotelOfMobs_count(), raffleOfMsgs_count()); 
    atomic_fetch_add(&iterations,1);
  }
  return 0;
}

// static bool testForever() {
//   printf("testForever\n");
//   assertInt (hotelOfMobs_bodyat(),  MOB_HEADER_SIZE);
//   assertInt (hotelOfMobs_recBlob(), MOB_GROSS_SIZE);
//   assertInt (hotelOfMobs_bodylen(), MOB_BODY_SIZE);
//   //assertInt (MOB_GROSS_SIZE, MOB_HEADER_SIZE + MOB_CODE_SIZE);
//   void stuffProg(Program * pProg) {
//     char * p = (char *) pProg;
//     injectOp(&p, *_rollCash);
//     injectFloatPair(&p, 3000000, 0.00001);
//     injectOp(&p, *_spawn);
//     injectOp(&p, *_end);
//     injectOp(&p, *_post);
//     injectOp(&p, *_end);
//   }
//   seed(50, 1000000, stuffProg); // Number of mobs, starting cash, spawn threshold
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


bool testCore() {
  return 
    testCode() &&
//    testForever() && 
    true || (showCore(), false);
}

bool core(void) { return bkt("core", init, testCore, cleanup); }

// Expt 2 result:
// Apply 1:20 murder rate per job. Thresh settles low with 1:16 spawn rate. 43% chance surviving til spawn - not.
// Its=200,000,000, Rent=1,000, thresh=8,115,720; Means: pop=488.25, spawnOdds=16.30214, childCash=1,675,726 msgCash=2,443,428, mobCash=1,466,057, totCash=3,909,484

