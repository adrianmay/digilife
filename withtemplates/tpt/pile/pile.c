#include "ipile.h"
#define STAGE 1
#include "XX_pile.h"
#include "XX.h"
#undef STAGE
#define STAGE 2
#include "Junk_pile.h"

Pilehead * headOfXXs = 0; 

bool openXXPile() { 
  bool v; 
  headOfXXs = openPile("XXs.pile", sizeof(XX), 10, YY, &v); 
  return v; 
} 

XX * getXX(XXIndex i) { 
  return (XX*)findInPile(headOfXXs, i.i); 
} 

void * withXX(XXIndex i, F_XX f, void * u) { 
  return withInPile(headOfXXs, i.i, (F)f, u);
} 

XXIndex allocXX(XX ** pNew) { 
  return (XXIndex) {.i=allocInPile(headOfXXs, (void**)pNew, 0, 0)}; 
} 

void freeXX(XXIndex i) {
  freeInPile(headOfXXs, i.i, 0, 0); 
} 

void closeXXPile(int rm) {
  closePile(headOfXXs, rm); 
} 

bool validXXIndex(XXIndex i) {
  return i.i != BAD_INDEX; 
} 

const XXIndex badXXIndex = (XXIndex) {BAD_INDEX}; 

Index countXXs() { 
  return countPop(headOfXXs); 
} 

