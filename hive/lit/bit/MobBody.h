#include "Mob_pile/1.h"

typedef struct {
  int effort; // Old thing for testing
  char code[40];
} PhyA;

typedef struct {
  int spawnThresh; // ito cash
  Cash pay; 
  double bid; 
  double fractionMsgOfMobSize;
  char pad[96];
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

