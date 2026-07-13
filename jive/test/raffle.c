
#include "h.h"
#include <unistd.h>
#include "misc/api.h"
//#include "perf/api.h"
#include "globals/api.h"
#include "Mess_raffle/ix.h"
#include "Mess_raffle/api.h"
#include "bit/Mess.h"


static bool extinct = false;
void onMessRaffle_extinct(void) { extinct = true; }

void showMess(MessIx i, Mess * pMess) {}


static void tock() { }

Cash cash;

int h, t, v, e;

/*
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

void apathy(MessTicket * pT) { (void)pT; }

void onMessRaffleConsume(MessIx i, MessTicket * pT) {
//raffleOfMesss.cancel(i);
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
*/


void empty(void) { while (!raffleOfMesss_empty()) { raffleOfMesss_draw(); }; }

void stuff1(void) {
  for (int a=0; a<1000; a++) {
    void stuff(Mess * pT) {
      pT->serial = a;
      pT->type = a%3 ? 'T' : 'H';  //Twice as many heads
    }
    raffleOfMesss_play(5000, 10, stuff);
  }
}

void stuff2(void) {
  for (int a=0; a<1000; a++) {
    void stuff(Mess * pT) {
      pT->serial=a;
      pT->type = a%2 ? 'T' : 'H';  //Twice as many heads
    }
    Weight w = a%2 ? 4 : 8;  //Twice as many heads
    raffleOfMesss_play(5000, w, stuff);
  }
}

void sample(void) {
  h=0; t=0; v=0; e=0;
  for (int a=0;a<100;a++) {
    //notifyCycles(1);
    bool res = raffleOfMesss_draw();
    //printf("In sample after draw: %d\n", res);
    if (!res) {
      printf("Sampled %d H, %d T, %d virgins and %d errors.\n", h, t, v, e);
      return;
    }
  }
  printf("Sampled %d H, %d T, %d virgins and %d errors.\n", h, t, v, e);
}


////////////////////////////////////////////////////////

int bored = 0;

#define BLOCKS 500

void stuffSerial(Mess * pT) { 
  static int ser=1;
  pT->type='m';
  pT->serial=ser++; 
}

void * produce(void * p) {
  (void)p;
  for (int a=0;a<BLOCKS;a++) {
    notifyCycles(300);
    sleepMs(randIntBelow(5));
    if (bored==1) break;
    raffleOfMesss_play(10, 1+randIntBelow(10), stuffSerial);
  }
  bored = 1;
  return 0;
}

bool testBlock() {
  pthread_t pid;
  pthread_create(&pid, 0, produce, 0);
  //sleepMs(200);
  //for (int a=0;a<BLOCKS;a++) {
  while (!bored) {
    sleepMs(1+randIntBelow(5));
    raffleOfMesss_draw();
    //raffleOfMesss.show();
  }
  pthread_join(pid, 0);
  return true;
}

bool testRaffle() {
  stuff1(); sample(); empty();
  stuff2(); sample(); empty();
  testBlock();
  return true;
}

void onMessRaffle_dispatch(MessIx i, Mess * pM, Cash cash, V claim, V unlock, V_C drop) {
  //printf("onMessRaffleDispatch\n");
  claim();
  unlock();
  if (pM->type=='H') h++;
  else if (pM->type=='T') t++;
  else if (pM->type=='V') v++;
  else e++;
  drop(0);  // Kill the message
}

static bool init(void) { onTestTock = tock; openGlobals(); raffleOfMesss_open(); return true; }
static void cleanup(void) { closeGlobals(Delete); raffleOfMesss_close(Delete); }
bool raffle(void) { return bkt("raffle", init, testRaffle, cleanup); }


