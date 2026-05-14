#include "types.h"

typedef void * (*F)(void * item, void * u);

typedef struct Pilehead Pilehead; 
Pilehead * openPile(const char * filename, Ix rec, Ix stp, Ix lim, bool * virgin);
void closePile(Pilehead * ph, FATE fate); 
Ix allocInPile(Pilehead * ph, void ** pNew, bool * pRecycled, void * ghost, int ghostlen); // Free block contents get copied to ghost
void * findInPile(Pilehead * ph, Ix i); // Just deref the index
void * withInPile(Pilehead * ph, Ix i, F f, void * u); // With derefed index
void freeInPile(Pilehead * ph, Ix i, void * ghost, int ghostlen); // Ghost gets inserted into free block for debugging
Ix countPop(Pilehead * ph );
Ix getUsr(Pilehead * ph); // Misc number
void setUsr(Pilehead * ph, Ix u);
void modUsr(Pilehead * ph, IxDiff u);
void showPile(Pilehead * ph, VP showSlot, bool onlyToUsr);
