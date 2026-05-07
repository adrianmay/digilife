#include <unistd.h>
#include "types.h"
#include "test.h"
#include "misc/h.h"
#include "globals/h.h"
//#include "MessBulk_pile/1.h"
//#include "MessBomb_pile/1.h"
//#include "Mess_hotel/Bulk.h"
//#include "Mess_hotel/Bomb.h"
#include "Mess_raffle/h.h"

static bool extinct = false;
void onMesssExtinct() { extinct = true; } 

static bool init() {
  openGlobals();
  raffleOfMesss.open();
  background(sweat_forever); // Got to do work to advance CPU time ...
  return true;
}

bool testRaffle() { return true; }
void cleanupRaffle() { closeGlobals(1); raffleOfMesss.close(1); }
bool raffle() { return bkt("raffle", init, testRaffle, cleanupRaffle); }


