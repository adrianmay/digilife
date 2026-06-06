#include "types.h"

bool openGlobals(void);
void closeGlobals(bool rm);

TockPrice tockPrice(void);
void notifyCycles(Cycles c);
Tocks tocksNow(void);
Cycles cyclesUntilTock(Tocks deadline);
Cycles cyclesPerTock();
