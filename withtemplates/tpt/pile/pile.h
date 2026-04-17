#include "types.h"

#if STAGE==1

typedef struct { Index i; } XXIndex; 

#else
#if STAGE==2

typedef void * (*F_XX)(XX * p, void * u); 
bool openXXPile() ; 
XX * getXX(XXIndex i) ; 
void * withXX(XXIndex i, F_XX f, void * u) ; 
XXIndex allocXX(XX ** pNew) ; 
void freeXX(XXIndex i) ;
void closeXXPile(int rm) ;
bool validXXIndex(XXIndex i) ;
Index countXXs() ; 
extern const XXIndex badXXIndex;


#else
#error 33
#endif
#endif

