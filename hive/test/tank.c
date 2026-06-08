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

static bool test1(void) {
  MobTact t1 = spawn(100, tGod, train1);
  MobTact t2 = spawn(200, tGod, train2);
  post(50, 1, t2, t1);

  return true;
}
static bool test(void) {
  return
    test1() &&
    true;
}

bool tank(void) { return bkt("tank", init, test, cleanup); }


