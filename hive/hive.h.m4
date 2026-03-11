include(pile.m4)dnl
#include "pile.h"

typedef uint64_t Cash;

PONDER_PILE(Block)dnl
typedef uint32_t BlockGhost;
PONDER_MEAP(Block)
typedef uint32_t BlockMeapGhost;

typedef uint64_t Tocks;

DECL_PILE(Block,`dnl
  Cash cash;
  BlockMeapIndex meap;
  uint32_t name;
')

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

