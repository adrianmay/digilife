#include "types.h"

#define GUESS_CYCLES_PER_TOCK 1000
#define GUESS_GROATS_PER_TOCK_PER_BYTE 0.1

bool openGlobals(void);
void closeGlobals(bool rm);

TockPrice tockPrice(void);
void notifyCycles(Cycles c);
Tocks tocksNow(void);
Cycles cyclesUntilTock(Tocks deadline);
Cycles cyclesPerTock();
void  onTock(void);
