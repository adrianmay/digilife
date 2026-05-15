// lang/lang.c
#include "h.h"
#include "structs.h"

void runMob(Mob * p) {
  for (int x=0, a=0; a < p->body.effort; a++) 
    x+=a;
}
