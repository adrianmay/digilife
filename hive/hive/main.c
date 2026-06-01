#include <stdio.h>
#include "args/h.h"
#include "bit/MsgTicket.h"
#include "bit/MobBody.h"

void onMobsExtinct(void) {}
void onMsgsExtinct(void) {}
void onThingsExtinct(void) {}

void showMobBody(MobBody * p) {
  printf("code=<binary>\n");

}

void showMsgTicket(MsgTicket * p) {
  printf("cpuBid=%lf, rcvr=%d\n", p->cpuBid, p->iRcvr.i);
}


int main(int argc, char **argv) {
  int res = parseArgs(argc, argv);
  printf("Hello from HIVE: %d\n", res);
  return 0;
}
