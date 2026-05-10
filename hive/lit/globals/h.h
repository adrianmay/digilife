#include "types.h"

bool openGlobals(void);
void closeGlobals(bool rm);

TockPrice tockPrice(void);
void updateTocks(void);
Tocks tocksNow(void);
Tocks nsUntilTock(Tocks deadline); 
Tocks nsAtTock(Tocks deadline); 
