#include "types.h"

typedef struct __attribute__((aligned(KILO))) { // This should be of a good size for alignment
  Index hdr; // Size of this header
  int fd; // File descriptor for underlying file
  Index rec; // Size of records
  Index stp; // How many pages to add when growing
  Index lim; // Max pages
  Index res; // File has space for this many records
  Index top; // Index of first untouched record
  Index fre; // Index of free record containing a pointer to the next one
  Index frn; // Num free slots
  char fn[MAX_FILENAME];             
  uint32_t usr; // Misc
} Pilehead;

                            
void * openGlobals_(uint64_t len, bool * virgin);
void closeGlobals_(int fd, bool rm);

Pilehead * openPile(const char * filename, Index rec, Index stp, Index lim);
void closePile(Pilehead * ph, bool rm);
Index allocInPile(Pilehead * ph, void ** pNew, void * ghost, int ghostlen); // Free block contents get copied to ghost
void * findInPile(Pilehead * ph, Index i); // Just deref the index
void freeInPile(Pilehead * ph, Index i, void * ghost, int ghostlen); // Ghost gets inserted into free block for debugging
void hidePile(Pilehead *); // Rename it for debugging
Index countPop(Pilehead * ph );
Index getUsr(Pilehead * ph); // Misc number
void setUsr(Pilehead * ph, Index u);
void modUsr(Pilehead * ph, int32_t u);

// Use this only after including something that defines the global structs

#define MAKEGLOBALS \
  VolatileGlobals vg; \
  PersistentGlobals * pg; \
  bool openGlobals() { \
    bool v; \
    pg = (PersistentGlobals *) openGlobals_(sizeof(PersistentGlobals), &v); \
    if (v) initVirginPersistentGlobals(); \
    initVolatileGlobals(); \
    return v; \
  } \
  void closeGlobals(bool rm) { closeGlobals_(pg->fd, rm); } \

// Between the two you can define the struct for TYP containing indices to itself:

#define MAKEPILE1(TYP) \
  typedef struct { Index i; } TYP##Index; 

#define MAKEPILE2(TYP, LIM) \
  Pilehead * headOf##TYP##s; \
  bool open##TYP##Pile() { return (headOf##TYP##s = openPile(#TYP "s.pile", sizeof(TYP), 10, LIM))->top==0; } \
  TYP * get##TYP(TYP##Index i) { return (TYP*)findInPile(headOf##TYP##s, i.i); } \
  TYP##Index alloc##TYP(TYP ** pNew) { return (TYP##Index) {.i=allocInPile(headOf##TYP##s, (void**)pNew, 0, 0)}; } \
  void free##TYP(TYP##Index i) { freeInPile(headOf##TYP##s, i.i, 0, 0); } \
  void close##TYP##Pile(bool rm) { closePile(headOf##TYP##s, rm); } \
  void hide##TYP##Pile() { hidePile(headOf##TYP##s); } \
  bool valid##TYP##Index(TYP##Index i) { return i.i != BAD_INDEX; } \
  const TYP##Index bad##TYP##Index = (TYP##Index) {BAD_INDEX}; \
  Index count##TYP##s() { return countPop(headOf##TYP##s); } \

