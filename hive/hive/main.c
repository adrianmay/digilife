#include "stdio.h"
#include "Junk_meap/h.h"

int main() { printf("Hello from HIVE\n"); }

Score getJunkScore(Junk * pJ) { return pJ->tocks; }
void  onNewJunk(JunkIndex iJ, Index hint) { }
void  onMoveJunk(Junk * pJ, JunkIndex to) { }
