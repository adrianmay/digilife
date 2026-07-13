#include "h.h"
#include "globals/api.h"
#include "Msg_raffle/ix.h"
#include "Mob_hotel/ix.h"
#include "Msg_raffle/api.h"
#include "Mob_hotel/api.h"
#include "tank/api.h"
#include "tank/Mob.h"
#include "tank/Msg.h"

bool testTank() {
  onTestTock = onTockTank;
  seed(5, 1000000, 5000);
  loop();
  return true;
}

static bool init(void) { 
  openGlobals(); hotelOfMobs_open(); raffleOfMsgs_open(); 
  printf("Sizes: mob=%f,msg=%f,tot=%f; Props: mob=%f,msg=%f\n", 
         SIZE_MOB, SIZE_MSG, SIZE_BOTH, MOB_PROP, MSG_PROP);
  return true;
}

static void cleanup(void) { closeGlobals(Delete); 
   raffleOfMsgs_close(Delete); hotelOfMobs_close(Delete); }
bool tank(void) { return bkt("raffle", init, testTank, cleanup); }
