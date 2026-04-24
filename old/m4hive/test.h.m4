include(pile.m4)dnl
#include "pile.h"

PONDER_PILE(Thing)dnl
typedef uint32_t ThingGhost;
PONDER_MEAP(Thing)dnl
typedef uint32_t ThingMeapGhost;


DECL_PILE(Thing,`dnl
  uint64_t cash;
  uint32_t name;
  ThingMeapIndex meap;
')

DECL_MEAP(Thing, `')




