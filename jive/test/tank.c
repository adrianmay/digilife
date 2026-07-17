#include <time.h>
#include <stdatomic.h>
#include "h.h"
#include "globals/api.h"
#include "Msg_raffle/ix.h"
#include "Mob_hotel/ix.h"
#include "Msg_raffle/api.h"
#include "Mob_hotel/api.h"
#include "tank/api.h"
#include "tank/Mob.h"
#include "tank/Msg.h"

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

bool testTank() {
  onTestTock = onTockTank;
  seed(5, 1000000, 20'000'000); // Number of mobs, starting cash, spawn threshold
  atomic_store(&iterations, 0);
  time_t start = time(NULL);
  work(0);
  //for (int64_t a=0;a<NUM_THREADS; a++) pthread_create(pids+a, 0, work, (void*)a);
  //for (int64_t a=0;a<NUM_THREADS; a++) pthread_join(pids[a], 0);
  time_t end = time(NULL);
  hotelOfMobs_show();
  raffleOfMsgs_show();
  printf("Took %'ld\n", end-start);
  return true;
}

bool tank(void) { return bkt("raffle", init, testTank, cleanup); }

// Expt 1 result:
// Its=100,000,000, Rent=1,000, thresh=20,000,000; Means: pop=975.21, spawnOdds=201.71355, childCash=7,667,410 msgCash=5,896,447, mobCash=3,537,868, totCash=9,434,315
// Cos cash in per rep = 1m - 5m/201.5 = 975186
// math/a.c shows that this is correct cos   

// Histogram of excess cash at want-to-spawn test:
// 0-100000       : 37210     
// 100000-200000  : 33267
// 200000-300000  : 29551
// 300000-400000  : 25703
// 400000-500000  : 21618
// 500000-600000  : 17984
// 600000-700000  : 13914
// 700000-800000  : 9898 
// 800000-900000  : 5918 
// 900000-1000000 : 1996

