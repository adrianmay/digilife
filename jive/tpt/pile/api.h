typedef void * (*P_XXP_P)(XX *, void *);
typedef void   (*V_XXI_XXP)(XXIx, XX*);

bool    pileOfXXs_open(void);
XXIx    pileOfXXs_alloc(XX ** pNew, bool * pRecycled);
XX *    pileOfXXs_get(XXIx i);
void    pileOfXXs_free(XXIx i);
void    pileOfXXs_close(Fate fate);
bool    pileOfXXs_ixValid(XXIx i);
Ix      pileOfXXs_count(void);
Ix      pileOfXXs_rec(void);
Ix      pileOfXXs_top(void);
Ix      pileOfXXs_getUsr(void); // Misc number
void    pileOfXXs_setUsr(Ix u);
void    pileOfXXs_modUsr(IxDiff u);
void    pileOfXXs_forAll(bool onlyToUsr, V_XXI_XXP action); // cos of meap
void    pileOfXXs_show(bool stopAtUsr); // cos of meap

extern void showXX(XXIx i, XX * pXX); //Provide this;

extern const XXIx badXXIx;

