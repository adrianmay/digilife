#include <unistd.h>
#include "types.h"
#include "test.h"
#include "misc/h.h"
#include "perf/h.h"
#include "globals/h.h"
#include "Mess_raffle/h.h"


void onMessRentCollected(Cash cash) { }
void onMessRentDefaulted(Cash cash) { }

void showMessTicket(MessTicket * p) {
  printf("type=%c\n", p->type);
}

static bool extinct = false;
void onMesssExtinct(void) { extinct = true; }

static bool init(void) {
  openGlobals();
  raffleOfMesss.open();
  return true;
}

MessTicket tkt = {'V'};
Cash cash;


WithMessTicket stuffTicket1(int a) {
  void stuff(MessTicket * pT) {
    pT->serial = a;
    pT->type = a%3 ? 'T' : 'H';  //Twice as many heads
  }
  return stuff;
}

// Equal weight, unequal types, loads of cash
void stuff1(void) {
  for (int a=0; a<1000; a++) {
    raffleOfMesss.enter(5000, 10, stuffTicket1(a));
  }
}

WithMessTicket stuffTicket2(int a) {
  void stuff(MessTicket * pT) {
    pT->serial=a;
    pT->type = a%2 ? 'T' : 'H';  //Twice as many heads
  }
  return stuff;
}

void stuff2(void) {
  for (int a=0; a<1000; a++) {
    Weight w = a%2 ? 4 : 8;  //Twice as many heads
    raffleOfMesss.enter(5000, w, stuffTicket2(a));
  }
  //raffleOfMesss.show();
}

void empty(void) {
  while (!raffleOfMesss.empty()) { raffleOfMesss.draw(&tkt, &cash); };
}

bool ch() {
  if (!raffleOfMesss.check()) {
    printf("Check failed\n");
    raffleOfMesss.show();
    return false;
  }
  return true;
}

void sample(void) {
  int h=0, t=0, v=0, e=0;
  for (int a=0;a<100;a++) {
    //notifyCycles(1);
    ch();
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

int bored = 0;


void apathy(MessTicket * pT) { (void)pT; }

void * produce(void * p) {
  (void)p;
  for (int a=0;a<20;a++) {
    sleepMs(1+randIntBelow(5));
    if (bored==1) break;
    raffleOfMesss.enter(10, 1+randIntBelow(10), apathy);
  }
  return 0;
}

bool testBlock() {
  pthread_t pid;
  MessTicket tick;
  Cash cash;
  pthread_create(&pid, 0, produce, 0);
  //sleepMs(200);
  for (int a=0;a<10;a++) {
    sleepMs(1+randIntBelow(5));
    raffleOfMesss.draw(&tick, &cash);
  }
  bored = 1;
  pthread_join(pid, 0);
  return true;
}

// bool testRecycling() {
//   MessTicket tick;
//   Cash cash;
//   Weight w;
//   raffleOfMesss.enter(5000, w, &tkt);
// }

bool testRaffle() {
//  stuff1(); ch(); sample(); ch(); empty();
//  stuff2(); ch(); sample(); ch(); empty();
  testBlock();
  return true;
}

bool raffle(void) { return bkt("raffle", init, testRaffle, cleanupRaffle); }


