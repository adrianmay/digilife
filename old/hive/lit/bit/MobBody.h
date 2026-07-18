#pragma once

#include <stdatomic.h>
#include "Mob_pile/1.h"
#include "Msg_pile/1.h"

typedef struct {
  int effort; // Old thing for testing
  char code[40];
} PhyA;

typedef struct {
  Cash spawnThresh; // ito cash
//  Cash payMsg; 
//  double bid; 
  char pad[96];
} PhyB;

typedef  union {
  PhyA a;
  PhyB b;
} PhyData;

enum {PHY_ERROR, PHY_GOD, PHY_A, PHY_B};

typedef struct {
  uint8_t phylum;
  PhyData p;
} MobBody;

void showMobBody(MobIx i, MobBody * p);

