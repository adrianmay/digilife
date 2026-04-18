#include "ipile/h.h"
#include "1.h"
#include "XX.h"
#include "2.h"

Pilehead * headOfXXs = 0; 
const XXIndex badXXIndex = (XXIndex) {BAD_INDEX}; 

bool      openXXPile()                        { bool v; headOfXXs = openPile("XXs.pile", sizeof(XX), 10, YY, &v); return v; } 
XX *      getXX(XXIndex i)                    { return (XX*)findInPile(headOfXXs, i.i); } 
void *    withXX(XXIndex i, F_XX f, void * u) { return withInPile(headOfXXs, i.i, (F)f, u); } 
XXIndex   allocXX(XX ** pNew)                 { return (XXIndex) {.i=allocInPile(headOfXXs, (void**)pNew, 0, 0)}; } 
void      freeXX(XXIndex i)                   { freeInPile(headOfXXs, i.i, 0, 0); } 
void      closeXXPile(FATE fate)              { closePile(headOfXXs, fate); headOfXXs = 0; } 
bool      validXXIndex(XXIndex i)             { return i.i != BAD_INDEX; } 
Index     countXXs()                          { return countPop(headOfXXs); } 

