
MAKERENT1(Block)

typedef struct { Index name; BlockRent rent; } Block;

MAKERENT2(Block, GIGA)

void mournBlockMeap(BlockMeap * pM) {}

bool rent() {
  printf("Fooooooo");
  openRentBlocks();
  Block * pB;
  BlockIndex iB = allocBlock(&pB);
  pB->rent.cash = 1000000;
  BlockMeap * pM;
  meapInsertBlockMeap(&pM, iB.i);
  return true;
}

