#include <unistd.h>
#include "types.h"
#include "test.h"
#include "misc/h.h"
#include "globals/h.h"
#include "Mess_raffle/h.h"

void showMessTicket(MessTicket * p) {
  printf("type=%c\n", p->type);
}

static bool extinct = false;
void onMesssExtinct(void) { extinct = true; } 

static bool init(void) {
  openGlobals();
  raffleOfMesss.open();
  background(sweat_forever); // Got to do work to advance CPU time ...
  return true;
}

MessTicket tkt = {'V' };
Cash cash;


// Equal weight, unequal types, loads of cash
void stuff1(void) {
  for (int a=0; a<10000; a++) {
    tkt.serial=a;
    tkt.type = a%3 ? 'T' : 'H';  //Twice as many heads
    raffleOfMesss.enter(5000, 10, &tkt);
  }
}

void stuff2(void) {
  for (int a=0; a<10000; a++) {
    tkt.serial=a;
    tkt.type = a%2 ? 'T' : 'H';  //Twice as many heads
    Weight w = a%2 ? 4 : 8;  //Twice as many heads
    raffleOfMesss.enter(5000, w, &tkt);
  }
  //raffleOfMesss.show();
}

void empty(void) {
  while (!raffleOfMesss.empty()) { raffleOfMesss.draw(&tkt, &cash); }; 
}

void sample(void) {
  int h=0, t=0, v=0, e=0;
  for (int a=0;a<100;a++) {
    bool res = raffleOfMesss.draw(&tkt, &cash);
    //printf("In sample after draw: %d\n", res);
    if (!res) {
      printf("Sampled %d H, %d T, %d virgins and %d errors.\n", h, t, v, e);
      return; 
    }
    if (tkt.type=='H') h++; 
    else if (tkt.type=='T') t++;
    else if (tkt.type=='V') v++;
    else e++;
  }
  printf("Sampled %d H, %d T, %d virgins and %d errors.\n", h, t, v, e);
}

void cleanupRaffle(void) { closeGlobals(DELETE); raffleOfMesss.close(DELETE); }

bool ch() {
  if (!raffleOfMesss.check()) {
    printf("Check failed\n");
    raffleOfMesss.show();
    return false;
  }
  return true;
}

bool testRaffle() { 
  stuff1(); ch(); sample(); ch(); empty();
  printf("Here\n");
  stuff2(); ch(); sample(); ch(); empty();
  return true; 
}

bool raffle(void) { return bkt("raffle", init, testRaffle, cleanupRaffle); }


