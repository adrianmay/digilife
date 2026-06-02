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

static void panicDump(XXIx i) {
  XX * pB = pileOfXXs.get(i);
  XXRafle * pR = &pB->body.raffle;
  printf("In panic: w=%'ld, i=%d, l=%'ld, s=%'ld, r=%'ld\n", peep(), i.i, pR->l, pR->s, pR->r );
  if (i.i==0) { return; }
  pop();
  panicDump(parent(i));
}

static Weight totWeightP(XXRafle * p) { return p->l + p->s + p->r; }

static Weight totWeightI(XXIx i) {
  if (i.i >= hotelOfXXs.count()) return 0;
  XX * pB = hotelOfXXs.get(i);
  XXRafle * pR = &pB->body.raffle;
  return totWeightP(pR);
}

static bool check_(const char * ctx, XXIx i) {
  XXRafle * pP = &pileOfXXs.get(      i )->body.raffle;
  XXRafle * pL = &pileOfXXs.get(left (i))->body.raffle;
  XXRafle * pR = &pileOfXXs.get(right(i))->body.raffle;
  bool res = (pP->l == 0 || (pP->l == totWeightP(pL) && check_(ctx, left (i))))
          && (pP->r == 0 || (pP->r == totWeightP(pR) && check_(ctx, right(i))));
  if (!res) { printf("Checking raffle from %s:\n", ctx); show(); panicDump(i); exit(2); }
  return res;
}

static bool checkM(const char * ctx) { return check_(ctx, (XXIx) {0}); }
static bool check() { return check_("user", (XXIx) {0}); }

//openXXRaffle() {openXXHotel();}

bool gottaQuitXX = false;

static Ix count(void) {
  return hotelOfXXs.count();
}

static void kill(void) {
  hotelOfXXs.kill();
  checkM("kill");
}

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

static XXIx enter(Cash cash, Weight w, XXTicket * pTicket) {
  char blah[40];
  lock();
  checkM("enter 1");
  bool wasEmpty = empty();
  XX * p;
  bool recycled;
  XXIx i = hotelOfXXs.alloc(cash, &p, &recycled);
  if (p->rent.cash>10000) { printf("Overrich 1 %d has %'ld from %'ld\n", i.i, p->rent.cash, cash); exit(1); }
  memcpy(&p->body.ticket, pTicket, sizeof(XXTicket));
  XXRafle * pRaf = &p->body.raffle;
  sprintf(blah, "enter 2 i=%d recyc=%b", i.i, recycled);
  checkM(blah);
  if (recycled) {
    //pRaf->l = totWeightI(left(i));
    //pRaf->r = totWeightI(right(i));
  } else pRaf->l = pRaf->r = 0;
  pRaf->s = w;
  propagateWeightUp(i, w);
  if (p->rent.cash>10000) { printf("Overrich 2 %d has %'ld\n", i.i, p->rent.cash); exit(1); }
  sprintf(blah, "enter 3 i=%d recyc=%b", i.i, recycled);
  checkM(blah);
  if (wasEmpty) pthread_cond_signal(&cond);
  unlock();
  return i;
}

static Cash cancel_(XXIx i) {
  XX * pB = pileOfXXs.get(i);
  XXRafle * pR = &pB->body.raffle;
  Weight w = pR->s;
  pR->s = 0;
  //When drawing, we'll treat a free slot like a proper node with a self-weight of zero and descend through it.
  propagateWeightUp(i, -w);
  Cash c = hotelOfXXs.rob(i); //Take all money so it soon gets freed...
  hotelOfXXs.kill();
  return c;
}

static Cash cancel(XXIx i) {
  lock();
  Cash c = cancel_(i);
  checkM("cancel");
  unlock();
  return c;
}

// Assumes there are tickets. Look out of onXXsExtinct
static Cash drawBelow(XXIx i, XXTicket * pTicket) {
  Cash c;
  XX * pB = pileOfXXs.get(i);
  XXRafle * pR = &pB->body.raffle;
  Weight target = peep();
  if (pR->l > 0 && target < pR->l) {
    push(target); 
    c = drawBelow(left(i), pTicket);
    pop();
    return c; 
  }
  target -= pR->l;
  if (target < pR->s) {
    c = pB->rent.cash;
    memcpy(pTicket, &pB->body.ticket, sizeof(XXTicket));
    cancel_(i);
    //printf("Returning cash=%ld from drawBelow\n", c);
    return c;
  }
  target -= pR->s;
  if (pR->r == 0) {
    printf("Bailing from drawBelow\n");
    panicDump(i);
    exit(10);
  }
  push(target);
  c = drawBelow(right(i), pTicket);
  pop();
  return c;
}

static Cash drawAssumeNotEmpty(XXTicket * pTicket) {
  XXIx i0 = (XXIx){0};
  Weight tw = totWeightI(i0);
  uint64_t w = randIntBelow(tw);
  //printf("Rolled w=%ld of %ld\n", w, tw);
  push(w);
  Cash c = drawBelow(i0, pTicket);
  pop();
  return c;
}

// If this returns false we're closing down the program
static bool draw(XXTicket * pTicket, Cash * pCash) {
  lock();
  checkM("draw 1");
  if (gottaQuitXX)    { gottaQuitXX=false; unlock(); return false; }
  else if (empty()) {
    pthread_cond_wait(&cond, &mutex);
  }
  if (gottaQuitXX)    { gottaQuitXX=false; unlock(); return false; }
  // Can't be empty
  *pCash = drawAssumeNotEmpty(pTicket);
  checkM("draw 2");
  unlock();
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

XXRaffle raffleOfXXs = { open, enter, cancel, empty, draw, close, show, count, check, kill, quitNow };


void showXXBody(XXIx i, XXBody * p) {
  printf("l=%'ld,s=%'ld,r=%'ld,⇑=%d,⇙=%d,⇘=%d,", p->raffle.l, p->raffle.s, p->raffle.r, parent(i).i, left(i).i, right(i).i);
  showXXTicket(&p->ticket);
}

