#include <pthread.h> 
#include "ipile/h.h"
#include "2.h"

Pilehead * headOfXXs = 0; 
const XXIndex  badXXIndex = (XXIndex) {BAD_INDEX}; 
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

extern void showXX(XX * pXX); //Provide this;
bool      openXXPile()                        { bool v; headOfXXs = openPile("XXs.pile", sizeof(XX), 10, YY, &v); return v; } 
XXIndex   allocXX(XX ** pNew)                 { 
  pthread_mutex_lock(&mutex);
  Index i = allocInPile(headOfXXs, (void**)pNew, 0, 0); 
  pthread_mutex_unlock(&mutex);
  return (XXIndex) {i};
} 
XX *      getXX(XXIndex i)                    { return (XX*)findInPile(headOfXXs, i.i); } 
void *    withXX(XXIndex i, F_XX f, void * u) { return withInPile(headOfXXs, i.i, (F)f, u); } 
void      freeXX(XXIndex i)                   { 
  pthread_mutex_lock(&mutex);
  freeInPile(headOfXXs, i.i, 0, 0); 
  pthread_mutex_unlock(&mutex);
} 
void      closeXXPile(FATE fate)              { closePile(headOfXXs, fate); headOfXXs = 0; } 
bool      validXXIndex(XXIndex i)             { return i.i != BAD_INDEX; } 
Index     countXXs()                          { return countPop(headOfXXs); } 
Index     getXXUsr()                          { return getUsr(headOfXXs); } 
void      setXXUsr(Index u)                   { setUsr(headOfXXs, u); } 
void      modXXUsr(IndexDiff u)               { modUsr(headOfXXs, u); }  // Make this atomic sometime
void      showXXPile()                        { showPile(headOfXXs, (VP)showXX); }

XXPile pileOfXXs = 
  { openXXPile
  , allocXX
  , getXX
  , withXX
  , freeXX
  , closeXXPile
  , validXXIndex
  , countXXs
  , getXXUsr
  , setXXUsr
  , modXXUsr
  , showXXPile
  };
