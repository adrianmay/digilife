
#include <pthread.h>
#include "types.h"
#include "ipile/api.h"
#include "ix.h"
#include "api.h"
#include "record.h"

#pragma GCC diagnostic ignored "-Wunused-function"

#if WW == 1
static pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
static void lock() { pthread_mutex_lock(&mutex); }
static void unlock() { pthread_mutex_unlock(&mutex); }
#else
static void lock() {}
static void unlock() {}
#endif

Pilehead * headOfXXs = 0;
const XXIx  badXXIx = (XXIx) {BAD_INDEX};
extern void showXX(XXIx i, XX * pXX); //Provide this;
static void showXX_(Ix i, void * p) { showXX( (XXIx){i}, (XX*)p); }

bool pileOfXXs_open(void) { 
  bool v; 
  headOfXXs = openPile("XXs.pile", sizeof(XX), 10, ZZ, &v); 
  return v; 
}

XXIx pileOfXXs_alloc(XX ** pNew, bool * pRecyc) { 
  lock(); 
  Ix i = allocInPile(headOfXXs, (void**)pNew, pRecyc, 0, 0);
  unlock(); 
  return (XXIx) {i};
}

XX * pileOfXXs_get(XXIx i) { 
  return (XX*)findInPile(headOfXXs, i.i); 
}

//void * withXX(XXIx i, F_XX f, void * u)       { return withInPile(headOfXXs, i.i, (F)f, u); }
void pileOfXXs_free(XXIx i) { 
  lock(); 
  freeInPile(headOfXXs, i.i, 0, 0);
  unlock(); 
}

void pileOfXXs_close(Fate fate) { 
  lock(); 
  closePile(headOfXXs, fate);
  headOfXXs = 0; 
  unlock(); 
}

bool pileOfXXs_ixValid(XXIx i)  { return (i.i & 0x7FFFFFFF) != 0x7FFFFFFF; }
Ix pileOfXXs_count(void)       { return countPop(headOfXXs); }
Ix pileOfXXs_top  (void)       { return topInPile(headOfXXs); }

Ix pileOfXXs_getUsr(void)       { return getUsr(headOfXXs); }
void pileOfXXs_setUsr(Ix u)     { setUsr(headOfXXs, u); }
void pileOfXXs_modUsr(IxDiff u) { modUsr(headOfXXs, u); }  // Make this atomic sometime
   
void pileOfXXs_forAll(bool u, V_XXI_XXP act) { 
  void a(Ix i, void * p) { act((XXIx){i}, (XX*)p); } 
  forAllPile(headOfXXs, u, a); 
}

void pileOfXXs_show(bool u) { showPile(headOfXXs, showXX_, u); }

