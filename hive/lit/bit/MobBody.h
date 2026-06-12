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

#define MOB_FLAG_RUNME    1
#define MOB_FLAG_BUSY     2
#define MOB_FLAG_BANKRUPT 4

typedef struct {
  Nick nick;
  uint8_t flags;
  uint8_t phylum;
  PhyData p;
} MobBody;

void showMobBody(MobIx i, MobBody * p);

