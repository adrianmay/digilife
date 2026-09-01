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
#include "core/ops.h"

#define BIRTH_CASH 1000'000'000L

void showWorld() {
  hotelOfMobs_show();
  raffleOfMsgs_show();
}

static bool init(void) {
  onTestTock = onTockCore;
    openGlobals(); hotelOfMobs_open(); raffleOfMsgs_open();
  //printf("Sizes: mob=%f,msg=%f,tot=%f; Props: mob=%f,msg=%f\n", SIZE_MOB, SIZE_MSG, SIZE_BOTH, MOB_PROP, MSG_PROP);
  return true;
}

static void cleanup(void) {
  raffleOfMsgs_close(Hide); hotelOfMobs_close(Hide); closeGlobals(Hide);
}

//#define NUM_THREADS 2
//static pthread_t pids[NUM_THREADS] = {0};

#define CORE_OUT_LEN 100
char out[CORE_OUT_LEN];
int outlen = CORE_OUT_LEN;

typedef struct {
  Program t;
  char e[CORE_OUT_LEN];
} Case;

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

void zapNicks() {
  char * r=out;
  while ((r=strchr(r+1, '='))) memset(r-8, 'x', 8);
}

static bool testSomeCases(const char * tag, Cash mobCash, Cash msgCash, Case cases[], int numCases) {
  bool res = true;
  for (int t=0;t<numCases;t++) {
    printf("####### test%s: #%d; mob: ", tag, t);
    void progStuffer(Program * prog) { memcpy((void*)prog, (void*)cases[t].t, sizeof(*prog)); };
    MobTact tact = create(mobCash, msgCash, progStuffer);
    char buf[20];
    hotelOfMobs_showsTact(buf, tact);
    printf("%s\n", buf);
    memset(out, 0, outlen);
    draw();
    zapNicks();
    if (0!=strcmp(out, cases[t].e)) {
      printf("test%s #%d Failed: want: '%s', got: '%s'\n", tag, t, cases[t].e, out);
      res = false; } }
  return res;
}

// static bool testSomeCases_(const char * tag, Cash mobCash, Cash msgCash, Case cases[], int numCases) {
//   Msg msg = {1};
//   Mob mob;
//   MobTact tMob = (MobTact){{8}, 0x1234abcd};
//   bool res = true;
//   for (int t=0;t<numCases;t++) {
//     printf("####### test%s: #%d\n", tag, t);
//     memcpy((char*)mob._.mortal.program, (char*)cases[t].t, sizeof(mob._.mortal.program));
//     runInCore(mobCash, msgCash, tMob, &mob, &msg);
//     if (0!=strcmp(out, cases[t].e)) {
//       printf("testCode #%d Failed: want: '%s', got: '%s'\n", t, cases[t].e, out);
//       res = false;
//     }
//   }
//   return res;
// }

// 4660 decimal:
#define OX1234f "\x00\xA0\x91\x45"

static bool testCode() {
  Case cases[] = {
    { PRS "Foo" NOP PRS "Bar" NOP END,                                                                         "FooBar"},
    { IFF YES PRS "A" NOP ELSIF NO  PRS "B" NOP END END,                                                       "A"},
    { IFF YES PRS "A" NOP ELSIF YES PRS "B" NOP END END,                                                       "A"},
    { IFF NO  PRS "A" NOP ELSIF YES PRS "B" NOP END END,                                                       "B"},
    { IFF NO  PRS "A" NOP ELSIF NO  PRS "B" NOP ELSIF YES  PRS "C" NOP END END,                                "C"},
    { IFF YES PRS "A" NOP ELSIF NO  PRS "B" NOP ELSIF YES  PRS "C" NOP END END,                                "A"},
    { IFF YES PRS "A" NOP ELSIF NO  PRS "B" NOP END  PRS "Z" NOP END,                                          "AZ"},
    { IFF YES PRS "A" NOP ELSIF NO  IFF YES PRS "C" NOP ELSIF NO  PRS "D" NOP END END  PRS "Z" NOP END,        "AZ"},
    { IFF NO  PRS "A" NOP ELSIF YES IFF YES PRS "C" NOP ELSIF NO  PRS "D" NOP END END  PRS "Z" NOP END,        "CZ"},
    { IFF NO  PRS "A" NOP ELSIF YES IFF NO  PRS "C" NOP ELSIF YES PRS "D" NOP END END  PRS "Z" NOP END,        "DZ"},
    { IFF NOT YES PRS "A" NOP ELSIF YES PRS "B" NOP END END,                                                   "B"},
    { IFF NOT NOT YES PRS "A" NOP ELSIF YES PRS "B" NOP END END,                                               "A"},
    { PRF ZERO END,                                                                                            "0.000000 "},
    { PRF ONE END,                                                                                             "1.000000 "},
    { PRF IMM OX1234f END,                                                                                     "4660.000000 "},
    { PRF ADD TWO TWO END,                                                                                     "4.000000 "},
    { PRF MUL TWO TWO END,                                                                                     "4.000000 "},
    { PRF MUL TWO ADD TWO ONE END,                                                                             "6.000000 "},
    { PRF MUL TWO INV ADD TWO ONE END,                                                                         "0.666667 "},
    { PRF ADD TWO NEG ADD TWO ONE END,                                                                         "-1.000000 "},
    { IFF GT ZERO ONE PRS "A" NOP ELSIF YES PRS "B" NOP END END,                                               "A"},
    { IFF LIKE MUL ONE INV ADD TWO TWO ONE TWO PRS "A" NOP ELSIF YES PRS "B" NOP END END,                      "B"},
    { PRP ME END,                                                                                              "xxxxxxxx=22"},
    { PRP PEER3 END,                                                                                           "xxxxxxxx=3"},
  };
  return testSomeCases("Code", 500'000, 500'000, cases, sizeof(cases)/sizeof(Case));
}

static bool testDisas() {
  Case cases[] = {
    { DISAS END,                                         "DISAS END "},
    { IFF NO PRF IMM OX1234f ELSIF YES DISAS END END,    "IFF NO PRF IMM 4660.000000 ELSIF YES DISAS END END "},
  };
  return testSomeCases("Disas", 4000, 4000, cases, sizeof(cases)/sizeof(Case));
}

static bool testBrokeMsg() {
  Case cases[] = {
    { PRF CYC END,                       "590.000000 "},
    //{ PRF CYC PRF CYC END,               "490.000000 480.000000 "},
    //{ PRF CYC PRF CYC PRF CYC END,       "490.000000 480.000000 470.000000 "},
    { PRF CSH END,                       "1000.000000 "},
  };
  return testSomeCases("BrokeMsg", 1000, 600, cases, sizeof(cases)/sizeof(Case));
}

static bool testSpawn() {
  Case cases[] = {
    { SPAWN PRP CHILD SPAWN PRP CHILD END,    "xxxxxxxx=29xxxxxxxx=30"},
  };
  return testSomeCases("Spawn", 8000, 8000, cases, sizeof(cases)/sizeof(Case));
}

// static bool testSpawnAndPost() {
//   printf("testSpawnAndPost\n");
//   Cash birthCash = BIRTH_CASH + randIntBelow(BIRTH_CASH);
//   void stuffProg(Program * pProg) {
//   }
//   seed(10, birthCash, stuffProg);
//   //MobTact tMob = (MobTact){{8}, 0x12345678};
//   //Mob mob;
//   //mob.phylum = PhyMortal;
//   //mob._.mortal.spawnThresh = 123;
//   //Program spawner = _spawn0 _post0 _end;
//   //memcpy((char*)mob._.mortal.program, spawner, sizeof(mob._.mortal.program));
//   // Make one real mob from this imaginary mob
//   //runInCore(birthCash, tMob, &mob, 0);
//   // Check the populations
//   Ix popMobs, popMsgs;
//   popMobs = hotelOfMobs_count();
//   assertInt(popMobs, 10);
//   popMsgs = raffleOfMsgs_count();
//   assertInt(popMsgs, 10);
//   // Inspect it
//   MobTact tMob0 = (MobTact){(MobIx){0},0};
//   Mob * pMob; Cash cash;
//   hotelOfMobs_grabIx(&tMob0, &pMob, &cash);
//   //showMob(iMob0, pMob);
//   //assertLong(pMob->_.mortal.spawnThresh, 123L);
//   Cash expect = birthCash*MOB_PROP;
//   assertLong(cash, expect);
//   hotelOfMobs_drop(tMob0.i, cash);
//   // Run the one mob in the hotel:
//   draw();
//   draw();
//   draw();
//   // Check the populations // raid
//   popMobs = hotelOfMobs_count();
//   assertInt(popMobs, 13);
//   popMsgs = raffleOfMsgs_count();
//   assertInt(popMsgs, 13);
//   //showMsgTicket((MsgTicketIx){0},0); printf("\n");
//   return true;
// }

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
//    testCode() &&
//    testDisas() &&
    testBrokeMsg() &&
//    testSpawn() &&
//    testForever() &&
    true || (showWorld(), false);
}

bool core(void) { return bkt("core", init, testCore, cleanup); }

// Expt 2 result:
// Apply 1:20 murder rate per job. Thresh settles low with 1:16 spawn rate. 43% chance surviving til spawn - not.
// Its=200,000,000, Rent=1,000, thresh=8,115,720; Means: pop=488.25, spawnOdds=16.30214, childCash=1,675,726 msgCash=2,443,428, mobCash=1,466,057, totCash=3,909,484

