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
  parseArgs(argc, argv);
  initEverything();
  bool suc =
//    wrap() &&
//    perf() &&
    pile() &&
    meap() &&
    hotel() &&
    globals() &&
//    raffle() &&
//    tank() &&
//    exec() &&
//    equi() &&
    true;
  return suc?0:1;
}

