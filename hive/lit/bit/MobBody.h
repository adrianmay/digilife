#include "Mob_pile/1.h"

typedef struct {
  int effort; // Old thing for testing
  char code[40];
} PhyA;

typedef struct {
  double oddsLazyByCash0;
  double oddsLazyByCash1; // Hope it learns to repro only when rich
  double msgCashByCash0; //How much of cash goes to msg
  double msgCashByCash1; //  For child msg use params after mutation
  Cash pay; 
  Cash spawnCash; 
  double bid; 
  double fractionMsgOfMobSize;
  // Children start with same cash as Adam. 
} PhyB;

typedef  union {
  PhyA a;
  PhyB b;
} PhyData;

typedef struct {
  char phylum;
  PhyData p;
} MobBody;

void showMobBody(MobIx i, MobBody * p);

