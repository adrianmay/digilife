#pragma GCC diagnostic ignored "-Winfinite-recursion"

#include <string.h>
#include <pthread.h>
#include "misc/h.h"
#include "XX_pile/1.h"
#include "XX_raffle/h.h"
#include "XX_pile/2.h"
#include "XX_hotel/h.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
static void lock() { pthread_mutex_lock(&mutex); }
static void unlock() { pthread_mutex_unlock(&mutex); }

static XXIx left  (XXIx i) {return ( XXIx ){ 2*i.i + 1 };}
static XXIx right (XXIx i) {return ( XXIx ){ 2*i.i + 2 };}
static XXIx parent(XXIx i) {return ( XXIx ){ (i.i-1)/2 };}
static bool isChild(XXIx i) { return i.i>0; }
static bool isChildRightChild(XXIx i) {return i.i%2==0; } //Whole approach could have fewer ifs

static Weight stack[100];
static int si=0;
static Weight peep()       { return stack[si-1]; }
static void push(Weight w) { stack[si++]=w; }
static void pop()          { si--; }

static void show(void) {
  hotelOfXXs.show();
}

static void panicDump_(XXIx i) {
  XX * pB = pileOfXXs.get(i);
  XXRafle * pRaf = &pB->body.raffle;
  printf("In panic: w=%'ld, i=%d, l=%'ld, s=%'ld, r=%'ld\n", peep(), i.i, pRaf->l, pRaf->s, pRaf->r );
  if (i.i==0) { return; }
  pop();
  panicDump_(parent(i));
}

static void panicDump(XXIx i) {
  printf("count: %d\n", pileOfXXs.count());
  panicDump_(i);
  show();
  abort();
}

static Weight totWeightP(XXRafle * p) { return p->l + p->s + p->r; }

static Weight totWeightI(XXIx i) {
  if (i.i >= pileOfXXs.top()) return 0;
  XX * pB = hotelOfXXs.get(i);
  XXRafle * pRaf = &pB->body.raffle;
  return totWeightP(pRaf);
}

static bool check_(const char * ctx, XXIx i) {
  if (pileOfXXs.top()==0) return true;
  XXRafle * pP = &pileOfXXs.get(i)->body.raffle;
  if (left(i).i < pileOfXXs.top()) {
    XXRafle * p = &pileOfXXs.get(left (i))->body.raffle;
    if (totWeightP(p) != pP->l) 
      panicDump(i);
  } else {
    if (pP->l != 0)
      panicDump(i);
  }
  if (right(i).i < pileOfXXs.top()) {
    XXRafle * p = &pileOfXXs.get(right(i))->body.raffle;
    if (totWeightP(p) != pP->r) 
      panicDump(i);
  } else {
    if (pP->r != 0)
      panicDump(i);
  }
  return true;
}

static bool checkM(const char * ctx) { return check_(ctx, (XXIx) {0}); }
static bool check() { return check_("user", (XXIx) {0}); }

//openXXRaffle() {openXXHotel();}

bool gottaQuitXX = false;

static Ix count(void) {
  return hotelOfXXs.count();
}

// static void raid(void) {
//   hotelOfXXs.raid();
//   checkM("raid");
// }

static void propagateWeightUp(XXIx i, Weight w) {
  // i's weight is already correct, this adjusts the ancestors
  if (!isChild(i)) return;
  XXIx iP = parent(i);
  if (isChildRightChild(i))
    pileOfXXs.get(iP)->body.raffle.r += w;
  else
    pileOfXXs.get(iP)->body.raffle.l += w;
  propagateWeightUp(iP, w);
}

static bool empty() {
  if (hotelOfXXs.count() == 0)
    return true;
  XXIx i0 = (XXIx){0};
  Weight tw = totWeightI(i0);
  if (tw==0)
    return true;
  return false;
}

static void play(Cash cash, Weight w, WithXXTicket stuffTicket) {
  printf("play: cash=%ld ", cash);
  char blah[40];
  checkM("enter1");
  bool wasEmpty = empty();
  XX * p;
  bool recycled;
  void stuffBody(XXBody * pBody) {
    stuffTicket(&pBody->ticket);
  }
  XXTact t = hotelOfXXs.admit(cash, stuffBody, &p, &recycled);
  //if (p->rent.cash>10000) { printf("Overrich 1 %d has %'ld from %'ld\n", i.i, p->rent.cash, cash); exit(1); }
  XXRafle * pRaf = &p->body.raffle; 
  if (!recycled) pRaf->s = pRaf->l = pRaf->r = 0; // Don't really need to zero s
  checkM(blah);
  lock();
  pRaf->s = w;
  propagateWeightUp(t.i, w);
  //if (p->rent.cash>10000) { printf("Overrich 2 %d has %'ld\n", i.i, p->rent.cash); exit(1); }
  sprintf(blah, "enter4 i=%d recyc=%b", t.i.i, recycled);
  checkM(blah);
  if (wasEmpty) pthread_cond_signal(&cond);
  unlock();
}


  //XX * p = pileOfXXs.get(i);
  //XXRafle * pRaf = &p->body.raffle;
  //if (pRaf->s == 0) abort();
  //Weight w = pRaf->s;
  //pRaf->s = 0;
  //propagateWeightUp(i, -w);

// Assumes there are tickets. Look out of onXXsExtinct

static bool draw();

static void drawBelow(XXIx i) {
  XX * pXX = hotelOfXXs.get(i);
  XXRafle * pRaf = &pXX->body.raffle;
  Weight target = peep();
  if (pRaf->l > 0 && target < pRaf->l) {
    push(target); 
    drawBelow(left(i));
    pop();
    return; 
  }
  void claim(void) {
    Weight w = pRaf->s;
    pRaf->s = 0;
    propagateWeightUp(i, -w);
  }
  void rob(void) {
    hotelOfXXs.rob(pXX);
    hotelOfXXs.raid();
  }
  target -= pRaf->l;
  if (target < pRaf->s) {
    onXXRaffleDispatch(i, pXX, claim, unlock, rob);
    return;
  }
  target -= pRaf->s;
  if (pRaf->r == 0) {
    printf("Bailing from drawBelow\n");
    panicDump(i);
    show();
    exit(10);
  }
  push(target);
  drawBelow(right(i));
  pop();
}

static void drawAssumeNotEmpty() {
  XXIx i0 = (XXIx){0};
  check(); 
  Weight tw = totWeightI(i0);
  uint64_t w = randIntBelow(tw);
  //printf("Rolled w=%ld of %ld\n", w, tw);
  push(w);
  drawBelow(i0);
  pop();
}

// If this returns false we're closing down the program
static bool draw() {
  lock();
  hotelOfXXs.raid();
  checkM("draw 1");
  if (gottaQuitXX)    { gottaQuitXX=false; unlock(); return false; }
  else if (empty()) {
    pthread_cond_wait(&cond, &mutex);
  }
  if (gottaQuitXX)    { gottaQuitXX=false; unlock(); return false; }
  // Can't be empty
  drawAssumeNotEmpty(); //unlocks
  checkM("draw 2");
  return true;
}

static bool open() {
  lock();
  bool ret = hotelOfXXs.open();
  unlock();
  return ret;
}
static void close(FATE f) {
  lock();
  hotelOfXXs.close(f);
  unlock();
}

static void quitNow() {
  gottaQuitXX = true;
  pthread_cond_signal(&cond);
}

static Cash rob(XX *  pXX) {
  return hotelOfXXs.rob(pXX);
}

// static Cash robUpTo(XXIx who, Cash limit) {
//   return hotelOfXXs.poorer(who, limit, Ono);
// }

XXRaffle raffleOfXXs = { open, play, rob, empty, draw, close, show, count, check, quitNow };

void showXXBody(XXIx i, XXBody * p) {
  printf("l=%-4ld s=%-4ld r=%-4ld ⇑=%-3d ⇙=%-3d ⇘=%-3d ", 
         p->raffle.l, p->raffle.s, p->raffle.r, 
         (i.i==0)?(-1):(parent(i).i), left(i).i, right(i).i);
  showXXTicket(&p->ticket);
}

void onXXHotelGoDie(XXIx i, XX * pXX) {
  Weight w = pXX->body.raffle.s;
  pXX->body.raffle.s = 0;
  propagateWeightUp(i, -w);
}
