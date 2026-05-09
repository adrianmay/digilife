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

MessTicket tkt = {'V' };
Cash cash;

// Equal weight, unequal types, loads of cash
void stuff1() {
  for (int a=0; a<100; a++) {
    tkt.serial=a;
    tkt.type = a%3 ? 'T' : 'H';  //Twice as many heads
    raffleOfMesss.enter(5000, 10, &tkt);
  }
}

void stuff2() {
  for (int a=0; a<10; a++) {
    tkt.serial=a;
    tkt.type = a%2 ? 'T' : 'H';  //Twice as many heads
    Weight w = a%2 ? 4 : 8;  //Twice as many heads
    raffleOfMesss.enter(5000, w, &tkt);
  }
  //raffleOfMesss.show();
}

void sample() {
  int h=0, t=0, v=0, e=0;
  for (int a=0; a<1000; a++) {
    bool res = raffleOfMesss.draw(&tkt, &cash);
    if (!res) return;
    if (tkt.type=='H') h++; 
    else if (tkt.type=='T') t++;
    else if (tkt.type=='V') v++;
    else e++;
  }
  printf("Sampled %d H, %d T, %d virgins and %d errors.\n", h, t, v, e);
}

void cleanupRaffle() { closeGlobals(1); raffleOfMesss.close(1); }

bool testRaffle() { 
  stuff1();
  sample();
  cleanupRaffle();
  init();
  stuff2();
  sample();
  cleanupRaffle();
  //init();
  //stuff3();
  //sample();
  return true; 
}

bool raffle() { return bkt("raffle", init, testRaffle, cleanupRaffle); }


