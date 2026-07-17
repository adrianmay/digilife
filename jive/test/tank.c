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
  seed(50, 1000000, 20'000'000); // Number of mobs, starting cash, spawn threshold
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

// Expt 2 result:
// Apply 1:20 murder rate per job. Thresh settles low with 1:16 spawn rate. 43% chance surviving til spawn - not.
// Its=200,000,000, Rent=1,000, thresh=8,115,720; Means: pop=488.25, spawnOdds=16.30214, childCash=1,675,726 msgCash=2,443,428, mobCash=1,466,057, totCash=3,909,484

