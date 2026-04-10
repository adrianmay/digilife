#include "test.h"

void B2V(B b) { (*b)(); }

bool nowt() { return true; }

bool bkt(B up, B along, V down) {
  bool suc;
  if (suc=(*up)()) {
    suc = (*along)();
    (*down)();
  }
  return suc;
}


int main() {
  srand(0);
  setlocale(LC_NUMERIC, "");
  x();
  bool suc = 
    globals() && 
    wrap() && 
    pile() && 
    trysleep() && 
    meap() && 
    rent() &&
    true;
  return suc?0:1;
}
