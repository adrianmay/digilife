#pragma once

#include "1.h"
#include "YY"

typedef void * (*F_XX)(XX * p, void * u);
typedef void (*XXVIP)(XXIx, XX*);

typedef bool    (*XXPileOpen)(void);
typedef XXIx    (*XXPileAlloc)(XX ** pNew, bool * pRecycled);
typedef XX *    (*XXPileGet)(XXIx i);
//typedef void *  (*XXPileWith)(XXIx i, F_XX f, void * u);
typedef void    (*XXPileFree)(XXIx i);
typedef void    (*XXPileClose)(FATE fate);
typedef bool    (*XXPileIxValid)(XXIx i);
typedef Ix      (*XXPileCount)(void);
// Types are same but this saves collisions:
typedef Ix      (*XXGetUsr)(void); // Misc number
typedef void    (*XXSetUsr)(Ix u);
typedef void    (*XXModUsr)(IxDiff u);
typedef void    (*XXPileForAll)(bool onlyToUsr, XXVIP action); // cos of meap
typedef void    (*XXPileShow)(bool stopAtUsr); // cos of meap

typedef struct {
  XXPileOpen open;
  XXPileAlloc alloc;
  XXPileGet get;
//  XXPileWith with;
  XXPileFree free;
  XXPileClose close;
  XXPileIxValid indexValid;
  XXPileCount count;
  XXPileCount top;
  XXGetUsr getUsr;
  XXSetUsr setUsr;
  XXModUsr modUsr;
  XXPileForAll forAll;
  XXPileShow show;
} XXPile;

extern XXPile pileOfXXs;

extern const XXIx badXXIx;

