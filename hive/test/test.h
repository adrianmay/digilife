#include <stdbool.h>
#include <stdio.h>
//#include <pthread.h>
//#include <stdlib.h>
//#include <locale.h>
//#include <signal.h>
//#include <stdint.h>

typedef void (*V)();
typedef bool  (*B)();
typedef bool (*BV)();

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

#define assertIntSufHex(VAR, VAL, SUF) \
  if (VAR != VAL) { \
    fprintf(stderr, "%s:%d: Expected: " #VAL "=%x ; Got: %x %s\n", __FILE__, __LINE__, VAL, VAR, SUF); \
    return false; \
  }


bool bkt(B up, B along, V down);
bool nowt();

bool globals();
bool wrap();
bool pile();
bool trysleep();
bool meap();
bool rent();

bool openGlobals();
void closeGlobals(bool);

