
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

static void showXX_(Ix i, void * p) {
  showXX( (XXIx){i}, (XX*)p); 
}

bool openXXPile(void) { 
  bool v; 
  headOfXXs = openPile("XXs.pile", sizeof(XX), 10, ZZ, &v); 
  return v; 
}

XXIx allocXX(XX ** pNew, bool * pRecyc) { 
  lock(); 
  Ix i = allocInPile(headOfXXs, (void**)pNew, pRecyc, 0, 0);
  unlock(); 
  return (XXIx) {i};
}

XX * getXX(XXIx i) { 
  return (XX*)findInPile(headOfXXs, i.i); 
}

//void * withXX(XXIx i, F_XX f, void * u)       { return withInPile(headOfXXs, i.i, (F)f, u); }
void freeXX(XXIx i) { 
  lock(); 
  freeInPile(headOfXXs, i.i, 0, 0);
  unlock(); 
}

void closeXXPile(Fate fate) { 
  lock(); 
  closePile(headOfXXs, fate);
  headOfXXs = 0; 
  unlock(); 
}

bool validXXIx(XXIx i)  { return (i.i & 0x7FFFFFFF) != 0x7FFFFFFF; }
Ix countXXs(void)       { return countPop(headOfXXs); }
Ix topOfXXs(void)       { return topInPile(headOfXXs); }

Ix getXXUsr(void)       { return getUsr(headOfXXs); }
void setXXUsr(Ix u)     { setUsr(headOfXXs, u); }
void modXXUsr(IxDiff u) { modUsr(headOfXXs, u); }  // Make this atomic sometime
   
void forAllXXPile(bool u, V_XXI_XXP act) { 
  void a(Ix i, void * p) { act((XXIx){i}, (XX*)p); } 
  forAllPile(headOfXXs, u, a); 
}

void showXXPile(bool u) { showPile(headOfXXs, showXX_, u); }

XXPile pileOfXXs =
  { openXXPile
  , allocXX
  , getXX
//  , withXX
  , freeXX
  , closeXXPile
  , validXXIx
  , countXXs
  , topOfXXs
  , getXXUsr
  , setXXUsr
  , modXXUsr
  , forAllXXPile
  , showXXPile
  };
