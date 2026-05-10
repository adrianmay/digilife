#include "1.h"
#include "YY"

typedef void * (*F_XX)(XX * p, void * u); 

typedef bool    (*XXPileOpen)(void); 
typedef XXIndex (*XXPileAlloc)(XX ** pNew, bool * pRecycled); 
typedef XX *    (*XXPileGet)(XXIndex i); 
typedef void *  (*XXPileWith)(XXIndex i, F_XX f, void * u); 
typedef void    (*XXPileFree)(XXIndex i);
typedef void    (*XXPileClose)(FATE fate);
typedef bool    (*XXPileIndexValid)(XXIndex i);
typedef Index   (*XXPileCount)(void); 
// Types are same but this saves collisions:
typedef Index   (*XXGetUsr)(void); // Misc number
typedef void    (*XXSetUsr)(Index u);
typedef void    (*XXModUsr)(IndexDiff u);
typedef void    (*XXPileShow)(void);

typedef struct {
  XXPileOpen open;
  XXPileAlloc alloc;
  XXPileGet get;
  XXPileWith with;
  XXPileFree free;
  XXPileClose close;
  XXPileIndexValid indexValid;
  XXPileCount count;
  XXGetUsr getUsr;
  XXSetUsr setUsr;
  XXModUsr modUsr;
  XXPileShow show;
} XXPile;

extern XXPile pileOfXXs;

extern const XXIndex badXXIndex;

