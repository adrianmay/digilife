#include "types.h"

bool openGlobals(void);
void closeGlobals(bool rm);

TockPrice tockPrice(void);
void updateTocks(void);
Tocks tocksNow(void);
Cycles cyclesUntilTock(Tocks deadline); 
Cycles cyclesAtTock(Tocks deadline); 
Cycles cyclesPerTock(); 
