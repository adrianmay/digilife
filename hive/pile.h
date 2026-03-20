#include <stdint.h>         
#include <stdbool.h>

#define BAD_INDEX UINT32_MAX
#define MAX_FILENAME 256
#define PAGE 4096    
#define KILO 1024           
#define MEGA (KILO*KILO)    
#define GIGA (MEGA*KILO)    
#define TERA (GIGA*KILO)    
#define B8 256ull           
#define B16 (B8*B8)         
#define B32 (B16*B16)       

typedef uint32_t Index;

typedef struct __attribute__((aligned(KILO))) { // This should be of a good size for alignment
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

                            
Pilehead * openPile(const char * filename, Index rec, Index stp, Index lim);
void closePile(Pilehead * ph, bool rm);
Index allocInPile(Pilehead * ph, void * prototype, void * ghost, int ghostlen);
void  * findInPile(Pilehead * ph, Index i);
void freeInPile(Pilehead * ph, Index i, void * ghost, int ghostlen);
void hidePile(Pilehead *);

#define MAKEPILE1(TYP) \
  typedef struct { Index i; } TYP##Index; 

#define MAKEPILE2(TYP, LIM) \
  Pilehead * headOf##TYP##s; \
  bool open##TYP##Pile() { return (headOf##TYP##s = openPile(#TYP "s.pile", sizeof(TYP), 10, LIM))->top==0; } \
  TYP * get##TYP(TYP##Index i) { return findInPile(headOf##TYP##s, i.i); } \
  TYP##Index alloc##TYP() { return (TYP##Index) {.i=allocInPile(headOf##TYP##s, &prototype##TYP, 0, 0)}; } \
  void free##TYP(TYP##Index i) { freeInPile(headOf##TYP##s, i.i, 0, 0); } \
  void close##TYP##Pile(bool rm) { closePile(headOf##TYP##s, rm); } \
  void hide##TYP##Pile() { hidePile(headOf##TYP##s); } \
  bool valid##TYP##Index(TYP##Index i) { return i.i != BAD_INDEX; }

 
