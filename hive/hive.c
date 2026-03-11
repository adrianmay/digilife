#include <stdio.h>
#include "hive.m4.h"
int main() { 
  bool virgin = openPileOfBlocks(false);
  openPileOfBlockMeaps(false);
  if (virgin) {
    BlockIndex bi = allocBlock(0);
    getBlock(bi)->cash = 100;

  } else
  {
  }
  if (openPileOfBlockMeaps(false)) {
    //virgin
    BlockIndex bi = allocBlock(0);
    getBlock(bi)->cash = 100;
    printf("Virgin\n");
    return 0; 
  } else {
    BlockIndex bi = {0};
    printf("Found: %d\n", getBlock(bi)->cash);
    return 0; 
  }
}

uint32_t newRandomName() { return -1; }
Tocks buyTocks(Cash c) { return c; }

BlockIndex newBlock(Cash total, Cash rent) {
  BlockIndex bi = allocBlock(0);
  Block * p = getBlock(bi);
  p->cash = total-rent;
  p->name = newRandomName();
  Tocks tocks = buyTocks(rent);
  MeapScore earliest;
  bool newEarliest = addToBlockMeap(bi, tocks, &earliest);
  // Just use Tocks and forget about abstracting MeapScore.
    
}
