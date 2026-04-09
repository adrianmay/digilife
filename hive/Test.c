#include <stdio.h>
#include <locale.h>
#include <signal.h>
#include "rent.h"

#define assertInt(VAR, VAL) \
  if (VAR != VAL) { \
    fprintf(stderr, "%s:%d: Expected: " #VAL "=%d ; Got: %d\n", __FILE__, __LINE__, VAL, VAR); \
    return false; \
  }

#define assertIntSuf(VAR, VAL, SUF) \
  if (VAR != VAL) { \
    fprintf(stderr, "%s:%d: Expected: " #VAL "=%d ; Got: %d %s\n", __FILE__, __LINE__, VAL, VAR, SUF); \
    return false; \
  }

#define assertIntHex(VAR, VAL) \
  if (VAR != VAL) { \
    fprintf(stderr, "%s:%d: Expected: " #VAL "=0x%x ; Got: 0x%x\n", __FILE__, __LINE__, VAL, VAR); \
    return false; \
  }

typedef bool  (*B)();
typedef void (*V)();
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

///////////////////////////////////////////////////////

#include "test/globals.c"
#include "test/wrap.c"
#include "test/pile.c"
#include "test/meap.c"
#include "test/rent.c"
#include "test/experiments.c"

///////////////////////////////////////////////////////
/// EXPERIMENTS

///////////////////////////////////////////////////////


int main() {
  setlocale(LC_NUMERIC, "");
  //x();
  bool suc = 
//    globals() && 
//    wrap() && 
//    pile() && 
//    trysleep() && 
    meap() && 
//    rent() &&
    true;
  return suc?0:1;
}
