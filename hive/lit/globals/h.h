#include "types.h"

bool openGlobals();
void closeGlobals(bool rm);

TockPrice tockPrice();
void updateTocks();
Tocks tocksNow();
Tocks nsUntilTock(Tocks deadline); 
Tocks nsAtTock(Tocks deadline); 
