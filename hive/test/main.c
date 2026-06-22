 #include <pthread.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "types.h"
#include "globals/h.h"
#include "perf/h.h"
#include "args/h.h"
#include "test.h"
#include "bit/MsgTicket.h"
#include "bit/MobBody.h"


void showMobBody(MobIx i, MobBody * p) {
  printf(" todo=%-3d phylum=%d ", i.i, p->phylum);
  switch (p->phylum) {
    case PHY_GOD:
      printf("God\n");
      break;
    case PHY_A:
      printf("effort=%d\n", p->p.a.effort);
      break;
    case PHY_B:
      printf("spawnThresh=%-4ld payMsg=%ld bid=%f\n", p->p.b.spawnThresh, p->p.b.payMsg, p->p.b.bid);
      break;
    default:
      printf("Unknown phylum\n");
  }
}

void showMsgTicket(MsgTicket * p) {
  printf("cpuBid=%lf, rcvr=", p->cpuBid);
  showMobTact(p->rcvr);
}

void B2V(B b) { (*b)(); }

bool nowt(void) { return true; }

bool bkt(const char * name, BV up, BV along, VV down) {
  printf("Testing %s\n", name);
  bool suc;
  if ((suc=(*up)())) {
    suc = (*along)();
    (*down)();
  }
  return suc;
}

void * sweat(void * p) {
  uint64_t z=1;
  for (int b=0;b<10;b++) {
    for (int a=0;a<99999999;a++) {
      z*=a;
    }
    sched_yield();
  }
  return (void*)z;
}

void * sweat_forever(void * p) { while(true) sweat(0); }
pthread_t background(void * (*f)(void *)) { pthread_t pid; pthread_create(&pid, 0, f, 0); return pid; }

///////  Move this to library:
//static void handler(int sig){ (void)sig; }
//static void handleSigusr1() {
//  struct sigaction sa;
//  memset(&sa, 0, sizeof(sa));
//  sa.sa_handler = handler;
//  sa.sa_flags = 0;
//  sigemptyset(&sa.sa_mask);
//  if (sigaction(SIGUSR1, &sa, NULL) == -1) perror("sigaction");
//}
void initEverything(void) {
//  handleSigusr1();
  //initPerf();
  srand(123);
  setlocale(LC_NUMERIC, "");
}
/////////////////////

VV onTestTock;
void onTock() {onTestTock();}

int main(int argc, char **argv) {
  parseArgs(argc, argv);
  initEverything();
  bool suc =
//    wrap() &&
//    globals() &&
//    perf() &&
//    pile() &&
//    meap() &&
//    hotel() &&
    raffle() &&
//    tank() &&
//    exec() &&
//    equi() &&
    true;
  return suc?0:1;
}

