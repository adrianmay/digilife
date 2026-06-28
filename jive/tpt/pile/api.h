typedef void * (*P_XXP_P)(XX *, void *);
typedef void   (*V_XXI_XXP)(XXIx, XX*);

typedef bool    (*XXPileOpen)(void);
typedef XXIx    (*XXPileAlloc)(XX ** pNew, bool * pRecycled);
typedef XX *    (*XXPileGet)(XXIx i);
typedef void    (*XXPileFree)(XXIx i);
typedef void    (*XXPileClose)(Fate fate);
typedef bool    (*XXPileIxValid)(XXIx i);
typedef Ix      (*XXPileCount)(void);
// Types are same but this saves collisions:
typedef Ix      (*XXGetUsr)(void); // Misc number
typedef void    (*XXSetUsr)(Ix u);
typedef void    (*XXModUsr)(IxDiff u);
typedef void    (*XXPileForAll)(bool onlyToUsr, V_XXI_XXP action); // cos of meap
typedef void    (*XXPileShow)(bool stopAtUsr); // cos of meap

typedef struct {
  XXPileOpen open;
  XXPileAlloc alloc;
  XXPileGet get;
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

