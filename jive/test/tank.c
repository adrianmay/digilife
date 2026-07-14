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

static _Atomic int iterations;

void * work(void * p) {
//  while(iterations < 100000 && draw())  {
  while(draw())  {
    //if (iterations % 1 == 0)
      printf("Tocks=%d Mobs=%d Msgs=%d\n", tocksNow(), hotelOfMobs_count(), raffleOfMsgs_count()); 
    atomic_fetch_add(&iterations,1);
  }
  return 0;
}

#define NUM_THREADS 10
static pthread_t pids[NUM_THREADS] = {0};

bool testTank() {
  onTestTock = onTockTank;
  seed(5, 1000000, 5000);
  atomic_store(&iterations, 0);
  hotelOfMobs_show();
  raffleOfMsgs_show();
//  time_t start = time(NULL);
  for (int64_t a=0;a<NUM_THREADS; a++) pthread_create(pids+a, 0, work, (void*)a);
  for (int64_t a=0;a<NUM_THREADS; a++) pthread_join(pids[a], 0);
//  time_t end = time(NULL);
  //printf("Took %'ld\n", end-start);
  return true;
}

bool tank(void) { return bkt("raffle", init, testTank, cleanup); }
