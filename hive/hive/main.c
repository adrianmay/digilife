#include "stdio.h"
#include "Junk_meap/1.h"
#include "Junk_meap/2.h"

int main() { printf("Hello from HIVE\n"); }

Score getJunkScore(Junk * pJ) { return pJ->tocks; }
void  onNewJunk(JunkIndex iJ, Index hint) { }
void  onMoveJunk(Junk * pJ, JunkIndex to) { }
