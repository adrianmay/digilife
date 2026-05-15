#include <stdio.h>
#include "bit/MsgTicket.h"
#include "args/h.h"

int main(int argc, char **argv) { 
  int res = parseArgs(argc, argv);
  printf("Hello from HIVE: %d\n", res); 
  return 0; 
}
 
void onMobsExtinct(void) {}
void onMsgsExtinct(void) {}
void onThingsExtinct(void) {}
 
void showMsgTicket(MsgTicket * p) {}
