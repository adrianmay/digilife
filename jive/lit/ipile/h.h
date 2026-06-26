// The MSB of the first byte of structs in a pile is used
// to indicate freeness of a block. Don't set it in blocks in use.
// After allocation, the first 32 bits are set to a random number
// with the MSB clear. This might be useful as a unique nick to
// distinguish the current occupant from the previous occupant of
// a given slot.
// TODO: Limit to 2,147,483,648 slots

typedef void * (*F)(void * item, void * u);

typedef struct Pilehead Pilehead;
Pilehead * openPile(const char * filename, Ix rec, Ix stp, Ix lim, bool * virgin);
void closePile(Pilehead * ph, Fate fate);
Ix allocInPile(Pilehead * ph, void ** pNew, bool * pRecycled, void * ghost, int ghostlen); // Free block contents get copied to ghost
void * findInPile(Pilehead * ph, Ix i); // Just deref the index
void * withInPile(Pilehead * ph, Ix i, F f, void * u); // With derefed index
void freeInPile(Pilehead * ph, Ix i, void * ghost, int ghostlen); // Ghost gets inserted into free block for debugging
Ix countPop(Pilehead * ph );
Ix topInPile(Pilehead * ph );
Ix getUsr(Pilehead * ph); // Misc number
void setUsr(Pilehead * ph, Ix u);
void modUsr(Pilehead * ph, IxDiff u);
void forAllPile(Pilehead * ph, bool onlyToUsr, VIP act);
void showPile(Pilehead * ph, VIP showSlot, bool onlyToUsr);
