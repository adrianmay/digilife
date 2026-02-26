include(pile.m4)dnl
#include "pile.h"

#define KILO 1024
#define MEGA (KILO*KILO)
#define GIGA (MEGA*KILO)
#define TERA (GIGA*KILO)
#define B8 256ull
#define B16 (B8*B8)
#define B32 (B16*B16)

typedef uint64_t Cash;

PONDER_PILE(Block)dnl
PONDER_MEAP(Block)
typedef uint32_t BlockGhost;
typedef uint64_t Tocks;

DECL_PILE(Block,`dnl
  Cash cash;
  BlockMeapIndex killer;'
  uint32_t name;
)

typedef uint32_t BlockMeapGhost;
DECL_MEAP(Block, `dnl
  Tocks expires;')

dnl typedef uint32_t ThingGhost;
dnl typedef uint32_t ThingNodeGhost;
dnl PONDER_LIST(Thing)dnl
dnl DECL_LIST(Thing,`dnl
dnl   uint64_t when;
dnl   BlockIndex who;'
dnl )dnl

dnl typedef uint32_t ThongGhost;
dnl typedef uint32_t ThongNodeGhost;
dnl PONDER_TREE(Thong)dnl
dnl DECL_TREE(Thong,`dnl
dnl   int thont;'
dnl )

