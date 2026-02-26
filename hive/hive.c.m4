include(pile.m4)
#include "hive.m4.h"

IMPL_PILE(Global, 1, 1)
Global prototypeGlobal = { 0 };

IMPL_PILE(Block, B32, KILO)
Block prototypeBlock = { 10 };
MeapScore meapScoreOfBlock(BlockMeap * p) { return p->expires; }

dnl IMPL_LIST(Thing, B32, KILO)
dnl Thing prototypeThing = { 20, 30 };

dnl #define SEPARATOR '.'
dnl IMPL_TREE(Thong, B32, KILO)
dnl Thong prototypeThong = { 20 };
dnl const char * nameOfThong(Thong *) { return "";}

IMPL_MEAP(Block, B32, KILO)
BlockMeap prototypeBlockMeap = { 10 };
