#include "types.h"

#define GUESS_CYCLES_PER_TOCK 1000

bool openGlobals(void);
void closeGlobals(bool rm);

TockPrice tockPrice(void);
void notifyCycles(Cycles c);
Tocks tocksNow(void);
Cycles cyclesUntilTock(Tocks deadline);
Cycles cyclesPerTock();
