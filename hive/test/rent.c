#include "test.h"
#include "rent.h"

MAKERENT1(Block)

typedef struct { Index name; BlockRent rent; } Block;

MAKERENT2(Block, GIGA)

void mournBlockMeap(BlockMeap * pM) {
  printf("");
}


bool testRent() {
  openGlobals();
  openRentBlocks();
  Block * pB;
  BlockIndex iB = allocBlock(&pB);
  pB->rent.cash = 1000000;
  BlockMeap * pM;
  meapInsertBlockMeap(&pM, iB.i);
  rentCollector(killBlocks);
  return true;
}

void cleanupRent() { closeGlobals(true); closeRentBlocks(true); }
bool rent() { return bkt(nowt, testRent, cleanupRent); }

