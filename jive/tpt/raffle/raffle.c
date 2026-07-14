#pragma GCC diagnostic ignored "-Winfinite-recursion"

#include <string.h>
#include <pthread.h>
#include "types.h"
#include "misc/api.h"
#include "globals/api.h"
#include "XXTicket_hotel/ix.h"
#include "XXTicket_hotel/api.h"
#include "ix.h"
#include "ticket.h"
#include "api.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
static void lock() { pthread_mutex_lock(&mutex); }
static void unlock() { pthread_mutex_unlock(&mutex); }

static XXTicketIx left  (XXTicketIx i) {return ( XXTicketIx ){ 2*i.i + 1 };}
static XXTicketIx right (XXTicketIx i) {return ( XXTicketIx ){ 2*i.i + 2 };}
static XXTicketIx parent(XXTicketIx i) {return ( XXTicketIx ){ (i.i-1)/2 };}
static bool isChild(XXTicketIx i) { return i.i>0; }
static bool isChildRightChild(XXTicketIx i) {return i.i%2==0; } //Whole approach could have fewer ifs

static Weight stack[100];
static int si=0;
static Weight peep()       { return stack[si-1]; }
static void push(Weight w) { stack[si++]=w; }
static void pop()          { si--; }

static Weight totWeightP(Weights * p) { return p->l + p->s + p->r; }

static Weight totWeightI(XXTicketIx i) {
  if (i.i >= hotelOfXXTickets_top()) return 0;
  XXTicket * pT = hotelOfXXTickets_get(i);
  Weights * pW = &pT->weights;
  return totWeightP(pW);
}

bool raffleOfXXs_empty() {
  if (hotelOfXXTickets_count() == 0)
    return true;
  XXTicketIx i0 = (XXTicketIx){0};
  Weight tw = totWeightI(i0);
  if (tw==0)
    return true;
  return false;
}

static void propagateWeightUp(XXTicketIx i, Weight w) {
  // i's weight is already correct, this adjusts the ancestors
  if (!isChild(i)) return;
  XXTicketIx iP = parent(i);
  XXTicket * pT = hotelOfXXTickets_get(iP);
  if (isChildRightChild(i)) pT->weights.r += w;
  else                      pT->weights.l += w;
  propagateWeightUp(iP, w);
}

void raffleOfXXs_show(void)  { hotelOfXXTickets_show(); }

void raffleOfXXs_play(Cash cash, Weight w, WithXX stuff) {
  //printf("play: cash=%ld ", cash);
  //char blah[40];
  //checkM("enter1");
  lock();
  bool wasEmpty = raffleOfXXs_empty();
  XXTicket * pT;
  bool recycled;
  void stuffTicket(XXTicket * p) { 
    stuff(&p->body); 
  }
  XXTicketTact t = hotelOfXXTickets_admit(cash, false, stuffTicket, &pT, &recycled);
  //if (p->rent.cash>10000) { printf("Overrich 1 %d has %'ld from %'ld\n", i.i, p->rent.cash, cash); exit(1); }
  Weights * pW = &pT->weights; 
  if (!recycled) {
    pW->s = pW->l = pW->r = 0; // Don't really need to zero s
  }
  //checkM(blah);
  pW->s = w;
  propagateWeightUp(t.i, w);
  //if (p->rent.cash>10000) { printf("Overrich 2 %d has %'ld\n", i.i, p->rent.cash); exit(1); }
  //sprintf(blah, "enter4 i=%d recyc=%b", t.i.i, recycled);
  //checkM(blah);
  if (wasEmpty) pthread_cond_signal(&cond);
  unlock();
}


static bool gottaQuitXX = false;

Ix raffleOfXXs_count(void) { return hotelOfXXTickets_count(); }

static void panicDump_(XXTicketIx i) {
  XXTicket * pTicket = hotelOfXXTickets_get(i);
  Weights * pW = &pTicket->weights;
  printf("In panic: w=%'ld, i=%d, l=%'ld, s=%'ld, r=%'ld\n", peep(), i.i, pW->l, pW->s, pW->r );
  if (i.i==0) { return; }
  pop();
  panicDump_(parent(i));
}

static void panicDump(XXTicketIx i) {
  printf("count: %d\n", hotelOfXXTickets_count());
  panicDump_(i);
  raffleOfXXs_show();
  DIE("panicDump %i", i.i);
}

static bool check_(const char * ctx, XXTicketIx i) {
  //printf("Checking raffle\n");
  if (hotelOfXXTickets_top() == 0) return true;
  Weights * pwCur = &hotelOfXXTickets_get(i)->weights;
  if (left(i).i < hotelOfXXTickets_top()) {
    Weights * pwL = &hotelOfXXTickets_get(left(i))->weights;
    if (totWeightP(pwL) != pwCur->l) 
      panicDump(i);
  } else {
    if (pwCur->l != 0)
      panicDump(i);
  } 
  if (right(i).i < hotelOfXXTickets_top()) {
    Weights * pwR = &hotelOfXXTickets_get(right(i))->weights;
    if (totWeightP(pwR) != pwCur->r) 
      panicDump(i);
  } else {
    if (pwCur->r != 0)
      panicDump(i);
  }
  return true;
}

//static bool checkM(const char * ctx) { return check_(ctx, (XXIx) {0}); }
bool raffleOfXXs_check() { 
  for (int a=0;a<hotelOfXXTickets_top();a++)
    check_("user", (XXTicketIx) {a}); 
  return true; //It'll have aborted otherwise
}


static void raid() {
  if (tocksNow()==1007) 
    printf("break\n");
  hotelOfXXTickets_raid();
}

// Assumes there are tickets. Look out of onXXHotel_extinct
static void drawBelow(XXTicketIx i) {
  //printf("Top of drawBelow: i=%d\n", i.i);
  XXTicket * pT = hotelOfXXTickets_get(i);
  Weights * pW = &pT->weights;
  Weight target = peep();
  if (pW->l > 0 && target < pW->l) {
    push(target); 
    drawBelow(left(i));
    pop();
    return; 
  }
  void claim(void) {
    Weight w = pW->s;
    pW->s = 0;
    propagateWeightUp(i, -w);
  }
  void drop(Cash cash) {
    hotelOfXXTickets_drop(i, cash);
  }
  target -= pW->l;
  if (target < pW->s) {
    Cash cash;
    hotelOfXXTickets_grabIx(i, &cash);
    onXXRaffle_dispatch((XXIx){i.i}, &pT->body, cash, claim, unlock, drop); 
    raid();
    return;
  }
  target -= pW->s;
  if (pW->r == 0) {
    printf("Bailing from drawBelow\n");
    //panicDump(i);
    raffleOfXXs_show();
    exit(10);
  }
  push(target);
  drawBelow(right(i));
  pop();
}

static void drawAssumeNotEmpty() {
  XXTicketIx i0 = (XXTicketIx){0};
  //check(); 
  Weight tw = totWeightI(i0);
  uint64_t w = randIntBelow(tw);
  //printf("Rolled w=%ld of %ld\n", w, tw);
  push(w);
  drawBelow(i0);
  pop();
}

// If this returns false we're closing down the program
bool raffleOfXXs_draw() {
  lock();
  raid();
  //checkM("draw 1");
  if (gottaQuitXX)  { gottaQuitXX=false; unlock(); return false; }
  if (raffleOfXXs_empty()) { pthread_cond_wait(&cond, &mutex); }
  if (gottaQuitXX)  { gottaQuitXX=false; unlock(); return false; }
  // Can't be empty
  drawAssumeNotEmpty(); //unlocks
  //checkM("draw 2");
  return true;
}

bool raffleOfXXs_open() {
  printf("raffleOfXXs_open: gottaQuit = %b\n", gottaQuitXX);
  lock();
  bool ret = hotelOfXXTickets_open();
  unlock();
  return ret;
}

void raffleOfXXs_close(Fate f) {
  lock();
  hotelOfXXTickets_close(f);
}

void raffleOfXXs_quit() {
  printf("raffleOfXXs_quit\n");
  abort();
  gottaQuitXX = true;
  pthread_cond_signal(&cond);
}

void showXXTicket(XXTicketIx i, XXTicket * pT) {
  XXTicket * p = hotelOfXXTickets_get(i);
  printf("l=%-4ld s=%-4ld r=%-4ld ⇑=%-3d ⇙=%-3d ⇘=%-3d ", 
         p->weights.l, p->weights.s, p->weights.r, 
         (i.i==0)?(-1):(parent(i).i), left(i).i, right(i).i);
  showXX((XXIx){i.i}, &p->body);
}

double raffleOfXXs_rec(void) { return hotelOfXXTickets_rec(); }
TockPrice raffleOfXXs_rent() { return tockPrice() * raffleOfXXs_rec(); }

void onXXTicketHotel_goDie(XXTicketIx i, XXTicket * pT) {
  Weight w = pT->weights.s;
  pT->weights.s = 0;
  propagateWeightUp(i, -w);
}

void onXXTicketHotel_rentCollected (Cash rent) {}
void onXXTicketHotel_rentDefaulted (Cash rent) {}
void onXXTicketHotel_extinct       (void) { onXXRaffle_extinct(); }

// static void panicDump_(XXIx i) {
//   XX * pB = pileOfXXs.get(i);
//   XXRafle * pRaf = &pB->body.raffle;
//   printf("In panic: w=%'ld, i=%d, l=%'ld, s=%'ld, r=%'ld\n", peep(), i.i, pRaf->l, pRaf->s, pRaf->r );
//   if (i.i==0) { return; }
//   pop();
//   panicDump_(parent(i));
// }

// static void panicDump(XXIx i) {
//   printf("count: %d\n", pileOfXXs.count());
//   panicDump_(i);
//   show();
//   die();
// }

// static bool check_(const char * ctx, XXIx i) {
//   if (pileOfXXs.top()==0) return true;
//   XXRafle * pP = &pileOfXXs.get(i)->body.raffle;
//   if (left(i).i < pileOfXXs.top()) {
//     XXRafle * p = &pileOfXXs.get(left (i))->body.raffle;
//     if (totWeightP(p) != pP->l)
//       panicDump(i);
//   } else {
//     if (pP->l != 0)
//       panicDump(i);
//   }
//   if (right(i).i < pileOfXXs.top()) {
//     XXRafle * p = &pileOfXXs.get(right(i))->body.raffle;
//     if (totWeightP(p) != pP->r)
//       panicDump(i);
//   } else {
//     if (pP->r != 0)
//       panicDump(i);
//   }
//   return true;
// }

//openXXRaffle() {openXXHotel();}
