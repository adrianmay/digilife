typedef void * (*F_XX)(XX * p, void * u); 


typedef bool    (*XXPileOpen)(); 
typedef XXIndex (*XXPileAlloc)(XX ** pNew); 
typedef XX *    (*XXPileGet)(XXIndex i); 
typedef void *  (*XXPileWith)(XXIndex i, F_XX f, void * u); 
typedef void    (*XXPileFree)(XXIndex i);
typedef void    (*XXPileClose)(FATE fate);
typedef bool    (*XXPileIndexValid)(XXIndex i);
typedef Index   (*XXPileCount)(); 
// Types are same but this saves collisions:
typedef Index   (*XXGetUsr)(); // Misc number
typedef void    (*XXSetUsr)(Index u);
typedef void    (*XXModUsr)(IndexDiff u);

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
} XXPile;

extern XXPile pileOfXXs;

extern const XXIndex badXXIndex;

