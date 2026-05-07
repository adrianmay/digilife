#include <unistd.h>
#include "types.h"
#include "test.h"
#include "misc/h.h"
#include "globals/h.h"
#include "Mess_raffle/h.h"

static bool extinct = false;
void onMesssExtinct() { extinct = true; } 

static bool init() {
  openGlobals();
  raffleOfMesss.open();
  background(sweat_forever); // Got to do work to advance CPU time ...
  return true;
}

MessTicket tkt;

// Equal weight, unequal types, loads of cash
void stuff1() {
  for (int a=0; a<10000; a++) {
    tkt.serial=a;
    tkt.type = a%3 ? 'T' : 'H';  //Twice as many heads
    raffleOfMesss.enter(5000, 10, &tkt);
  }
}

void sample1() {
  int h=0, t=0;
  for (int a=0; a<1000; a++) {
    raffleOfMesss.draw(&tkt);
    if (tkt.type=='H') h++; else t++;
  }
  printf("Sampled %d H and %d T\n", h, t);
}

bool testRaffle() { 
  stuff1();
  sample1();
  return true; 
}

void cleanupRaffle() { closeGlobals(1); raffleOfMesss.close(1); }
bool raffle() { return bkt("raffle", init, testRaffle, cleanupRaffle); }


