#include "test.h"
#include "Test_timer/h.h"




static bool init() { initTestTimer(); return true; }
static void cleanup() { unitTestTimer(); }

static bool test() { return true; }

bool timer() { return bkt("timer", init, test,cleanup); }
