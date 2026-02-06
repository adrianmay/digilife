#include <stdio.h>
#include "hive.m4.h"
int main() { 
  if (openPileOfBlocks(false)) {
    //virgin
    BlockIndex bi = allocBlock(0);
    getBlock(bi)->cash = 100;
    printf("Virgin\n");
    return 3; 
  } else {
    BlockIndex bi = {0};
    printf("Found: %d\n", getBlock(bi)->cash);
  }
}
