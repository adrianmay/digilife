#include <locale.h>
#include "h.h"
#include "args/api.h"

bool nowt(void) { return true; }

bool bkt(const char * name, B up, B along, V down) {
  printf("Testing %s\n", name);
  bool suc;
  if ((suc=(*up)())) {
    suc = (*along)();
    (*down)();
  }
  return suc;
}

void initEverything(void) {
//  handleSigusr1();
  //initPerf();
  srand(123);
  setlocale(LC_NUMERIC, "");
}

V onTestTock;
void onTock() {onTestTock();}

int main(int argc, char **argv) {
  printf("STARTING TESTS ... \n");
  parseArgs(argc, argv);
  printf(" A\n");
  initEverything();
  printf(" B\n");
  bool suc =
//    wrap() &&
//    perf() &&
//    globals() &&
//    pile() &&
//    meap() &&
//    hotel() &&
//    raffle() &&
    core() &&
//    exec() &&
//    equi() &&
    true; 
  printf("TESTS %s\n", suc ? "SUCCEEDED" : "FAILED");
  return suc?0:1;
}

