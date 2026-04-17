#include "types.h"

typedef struct Pilehead Pilehead; 
Pilehead * openPile(const char * filename, Index rec, Index stp, Index lim, bool * virgin);
void closePile(Pilehead * ph, int rm); // 1->delete, 2->hide
Index allocInPile(Pilehead * ph, void ** pNew, void * ghost, int ghostlen); // Free block contents get copied to ghost
void * findInPile(Pilehead * ph, Index i); // Just deref the index
void * withInPile(Pilehead * ph, Index i, F f, void * u); // With derefed index
void freeInPile(Pilehead * ph, Index i, void * ghost, int ghostlen); // Ghost gets inserted into free block for debugging
Index countPop(Pilehead * ph );
Index getUsr(Pilehead * ph); // Misc number
void setUsr(Pilehead * ph, Index u);
void modUsr(Pilehead * ph, int32_t u);
