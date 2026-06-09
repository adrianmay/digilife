#include <stdlib.h>
#include <string.h>
#include "test.h"
#include "tank/h.h"

static bool init(void) { openTank();  return true; }

static void cleanup(void) { closeTank(HIDE); }

CostAndResult runner(Mob * pMob) {
  return (CostAndResult) {1, 0};
}

void train1(MobBody * pMB) {
  pMB->phylum = 'a';
  strcpy(pMB->p.a.code,"1");
}

void train2(MobBody * pMB) {
  pMB->phylum = 'a';
  strcpy(pMB->p.a.code,"2");
}

WithPayload stuffPload(int a) {  
  void stff(MsgPayload * pP) {  
    printf("Consider yourself stuffed %d", a);
  }
  return stff;
}

static bool test1(void) {
  MobTact t1 = spawn(100, tInvestor, train1);
  MobTact t2 = spawn(200, tInvestor, train2);
  MsgIx iMsg1 = post(50, 1, t2, t1, stuffPload(1));
  MsgIx iMsg2 = post(60, 1, t1, t2, stuffPload(2));
  printf("MsgIx: %d\n", iMsg1.i);
  printf("MsgIx: %d\n", iMsg2.i);
  return true;
}
static bool test(void) {
  return
    test1() &&
    true;
}

bool tank(void) { return bkt("tank", init, test, cleanup); }


