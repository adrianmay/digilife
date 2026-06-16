#include <unistd.h>
#include "types.h"
#include "test.h"
#include "misc/h.h"
#include "perf/h.h"
#include "globals/h.h"
#include "Mess_raffle/h.h"

bool onMessRaffleApprove(MessIx i, MessTicket * pTicket) {
  return true;
}


void onMessRentCollected(Cash cash) { }
void onMessRentDefaulted(Cash cash) { }
extern void onMessRaffleGoDie(MessIx i) { (void)i; }

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
    void stuff(MessTicket * pT) {
      pT->serial = a;
      pT->type = a%3 ? 'T' : 'H';  //Twice as many heads
    }
    raffleOfMesss.play(5000, 10, stuff);
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
    raffleOfMesss.play(5000, w, stuffTicket2(a));
  }
  //raffleOfMesss.show();
}

void empty(void) {
  while (!raffleOfMesss.empty()) { raffleOfMesss.draw(); };
}

bool ch() {
  if (!raffleOfMesss.check()) {
    printf("Check failed\n");
    raffleOfMesss.show();
    return false;
  }
  return true;
}

int h, t, v, e;

void onMessRaffleConsume(MessIx i, MessTicket * pT) {
  if (pT->type=='H') h++;
  else if (pT->type=='T') t++;
  else if (pT->type=='V') v++;
  else e++;
}


void sample(void) {
  h=0; t=0; v=0; e=0;
  for (int a=0;a<100;a++) {
    //notifyCycles(1);
    ch();
    bool res = raffleOfMesss.draw();
    //printf("In sample after draw: %d\n", res);
    if (!res) {
      printf("Sampled %d H, %d T, %d virgins and %d errors.\n", h, t, v, e);
      return;
    }
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
    raffleOfMesss.play(10, 1+randIntBelow(10), apathy);
  }
  return 0;
}

bool testBlock() {
  pthread_t pid;
  pthread_create(&pid, 0, produce, 0);
  //sleepMs(200);
  for (int a=0;a<10;a++) {
    sleepMs(1+randIntBelow(5));
    raffleOfMesss.draw();
  }
  bored = 1;
  pthread_join(pid, 0);
  return true;
}

bool testRaffle() {
  stuff1(); ch(); sample(); ch(); empty();
  stuff2(); ch(); sample(); ch(); empty();
  testBlock();
  return true;
}

bool raffle(void) { return bkt("raffle", init, testRaffle, cleanupRaffle); }


