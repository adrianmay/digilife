#include <pthread.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "globals/h.h"
#include "test.h"

void B2V(B b) { (*b)(); }

bool nowt() { return true; }

bool bkt(const char * name, B up, B along, V down) {
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
      sched_yield();
      z*=a;
    }
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
void initEverything() {
//  handleSigusr1();
  srand(0);
  setlocale(LC_NUMERIC, "");
}
/////////////////////

int main() {
  initEverything();
  bool suc = 
//    perf() && 
    wrap() && 
    globals() && 
    pile() && 
    meap() && 
    hotel() &&
    true;
  return suc?0:1;
}
