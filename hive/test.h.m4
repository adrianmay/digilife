include(pile.m4)dnl
#include "pile.h"

PONDER_PILE(Thing)dnl
typedef uint32_t ThingGhost;
PONDER_MEAP(Thing)dnl
typedef uint32_t ThingMeapGhost;


DECL_PILE(Thing,`dnl
  uint32_t name;
  uint32_t cash;
  ThingMeapIndex meap;
')

typedef uint64_t Tocks;

DECL_MEAP(Thing, `dnl
  Tocks expires;')




