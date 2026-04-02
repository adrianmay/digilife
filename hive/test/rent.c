
MAKERENT1(Block)

typedef struct { Index name; BlockRent rent; } Block;

MAKERENT2(Block, GIGA)

bool rent() {
  return true;
}

