#define XX_PILE_HAS_FREE ZZ

typedef void * (*F_XX)(XX * p, void * u); 

typedef bool    (*XXPileOpen)(); 
typedef XXIndex (*XXPileAlloc)(XX ** pNew); 
typedef XX *    (*XXPileGet)(XXIndex i); 
typedef void *  (*XXPileWith)(XXIndex i, F_XX f, void * u); 
#if XX_PILE_HAS_FREE
typedef void    (*XXPileFree)(XXIndex i);
#endif
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
#if XX_PILE_HAS_FREE 
  XXPileFree free;
#endif
  XXPileClose close;
  XXPileIndexValid indexValid;
  XXPileCount count;
  XXGetUsr getUsr;
  XXSetUsr setUsr;
  XXModUsr modUsr;
} XXPile;

extern XXPile pileOfXXs;

extern const XXIndex badXXIndex;

