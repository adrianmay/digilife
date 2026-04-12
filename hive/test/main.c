#include <pthread.h>
#include <locale.h>
#include <stdlib.h>
#include "globals.h"
#include "test.h"

void B2V(B b) { (*b)(); }

bool nowt() { return true; }

bool bkt(B up, B along, V down) {
  bool suc;
  if ((suc=(*up)())) {
    suc = (*along)();
    (*down)();
  }
  return suc;
}

void * sweat(void *) {
  for (int b=0;b<10;b++) {
    uint64_t z=1;
    for (int a=0;a<99999999;a++)  
      z*=a;
  }
  return 0;
}

void * sweat_forever(void *) { while(true) sweat(0); }
void background(void * (*f)(void *)) { pthread_t pid; pthread_create(&pid, 0, f, 0); }

int main() {
  srand(0);
  setlocale(LC_NUMERIC, "");
  printf("process_age_costs_in_pence_per_second: %f\n", process_age_costs_in_pence_per_second);
  bool suc = 
//    sleep() && 
    wrap() && 
//    pile() && 
//    meap() && 
//    globals() && 
//    rent() &&
    true;
  return suc?0:1;
}
