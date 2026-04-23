#include "types.h"

typedef bool (*Worker)(Nanosecs * pNsRel);

void initXXTimer();
void workOnXXTimer(Worker worker);
void loopOnXXTimer(Worker worker);
