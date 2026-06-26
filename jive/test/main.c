#include <locale.h>
#include "h.h"
#include "args/h.h"

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

void initEverything(void) {
//  handleSigusr1();
  //initPerf();
  srand(123);
  setlocale(LC_NUMERIC, "");
}

int main(int argc, char **argv) {
  parseArgs(argc, argv);
  initEverything();
  bool suc =
//    wrap() &&
//    globals() &&
//    perf() &&
    pile() &&
    meap() &&
//    hotel() &&
//    raffle() &&
//    tank() &&
//    exec() &&
//    equi() &&
    true;
  return suc?0:1;
}

