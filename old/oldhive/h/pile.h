/*
This is pile.h: the header for a C file that provides persistent fixed-block-size allocators.  
A "pile" is a file mapped into memory containing a Pilehead followed by an array of blocks
  of a specified size. The stretch of virtual memory into which the file is mapped is huge in 
  order to allow for any plausible growth. The upper end of that virtual memory will not be
  backed by physical RAM until the pile grows that large. A high watermark is remembered 
  and when it's overstepped the file grows and more RAM is assigned. 
There's a linked list of used-then-freed blocks held together by indices
  in the freed blocks.
Calling code refers to blocks by their 32 bit indices in the array
  rather than by 64-bit pointers because it requires less memory.

There's also a simpler thing for storing one instance of an arbitrary struct in a memory mapped file.   
  This is used for global variables that must persist between invocations.
*/

#include "types.h" // Some integer type aliases and constants are defined in here. Also some standard C headers are included.

// This is the simple thing for globals.....

void * openGlobals_(uint64_t len, bool * virgin);
void closeGlobals_(int fd, bool rm);
// Use this macro only after including something that defines the global structs and initialising functions
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

//Now for the proper pile.....

// The header for a pile:
typedef struct __attribute__((aligned(KILO))) { // This should be of a good size for alignment
  Index hdr; // Size of this header
  int fd; // File descriptor for mapped file
  Index rec; // Size of records
  Index stp; // How many 4k pages to add when growing
  Index lim; // Max pages
  Index res; // File has space for this many records
  Index top; // Index of first untouched record, i.e., high watermark
  Index fri; // Index of recently freed record
  Index fro; // Index of next free record to be used in alloc
  _Atomic Index frn; // Num free slots
  char fn[MAX_FILENAME]; // So we can offer to delete it upon closing           
  uint32_t usr; // Misc persistent variable about this pile.
} Pilehead;
                            
// Forward references of stuff in the corresponding C file:
// The full pile:
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

// What follows are comfort macros taking a type (usually a struct) and making versions of the
//   main pile functions specialised to that type and choosing defaults for some parameters.
// Between using these two macros you can define the struct for TYP containing indices to itself:

#define MAKEPILE1(TYP) \
  typedef struct { Index i; } TYP##Index; 

#define MAKEPILE2(TYP, LIM) \
  Pilehead * headOf##TYP##s = 0; \
  bool open##TYP##Pile() { bool v; headOf##TYP##s = openPile(#TYP "s.pile", sizeof(TYP), 10, LIM, &v); return v; } \
  TYP * get##TYP(TYP##Index i) { return (TYP*)findInPile(headOf##TYP##s, i.i); } \
  typedef void * (*F_##TYP)(TYP * p, void * u); \
  void * with##TYP(TYP##Index i, F_##TYP f, void * u) { return withInPile(headOf##TYP##s, i.i, (F)f, u); } \
  TYP##Index alloc##TYP(TYP ** pNew) { return (TYP##Index) {.i=allocInPile(headOf##TYP##s, (void**)pNew, 0, 0)}; } \
  void free##TYP(TYP##Index i) { freeInPile(headOf##TYP##s, i.i, 0, 0); } \
  void close##TYP##Pile(int rm) { closePile(headOf##TYP##s, rm); } \
  bool valid##TYP##Index(TYP##Index i) { return i.i != BAD_INDEX; } \
  const TYP##Index bad##TYP##Index = (TYP##Index) {BAD_INDEX}; \
  Index count##TYP##s() { return countPop(headOf##TYP##s); } \

