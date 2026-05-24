// core/h.h

#include "perf/h.h"
#include "Mob_hotel/structs.h"

typedef int CoreHandle;
CoreHandle newCore();
void delCore(CoreHandle core);
bool quitting();
Alarm * coreAlarm(CoreHandle ch);
void forceYield(CoreHandle ch);

void runMob(Mob * p);
