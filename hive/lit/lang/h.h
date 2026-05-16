// lang/h.h

#include "Mob_hotel/structs.h"

typedef int CoreHandle;
CoreHandle newCore();
void delCore(CoreHandle core);
bool quitting();

void runMob(Mob * p);
