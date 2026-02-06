include(pile.m4)
#include "pile.h"

#define KILO 1024
#define MEGA (KILO*KILO)
#define GIGA (MEGA*KILO)
#define TERA (GIGA*KILO)
#define B8 256ull
#define B16 (B8*B8)
#define B32 (B16*B16)

typedef uint64_t Cash;

PONDER_PILE(Block)
typedef uint32_t BlockGhost;
DECL_PILE(Block,
Cash cash;
)

PONDER_PILE(Kill)
typedef uint32_t KillGhost;
DECL_PILE(Kill,
uint64_t when;
BlockIndex who;
)



