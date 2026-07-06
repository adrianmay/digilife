#include <string.h>
#include <stdlib.h>
#include "h.h"
#include "Junk_meap/ix.h"
#include "Junk_meap/api.h"
#include "bit/Junk.h"

void  onJunkMeapNew(Junk * pJ, Ix hint) { pJ->hint=hint;}
void  onJunkMeapMove(Junk * pJ, JunkIx to) { }

#define assertWholeMeap(pExp, N) { \
  printf("Meap setup %d: ", setupNum); \
  Ix tot = meapOfJunks_getUsr(); \
  assertInt(tot, N); \
  for (uint32_t a = 0; a<N; a++) { \
    Junk * v = getJunk((Junk) {a}); \
    assertIntHex(v->tocks,pExp[a]); \
  } \
}

#define assertIntM(A, B) {  \
  char s[30]; \
  sprintf(s, "with meap setup %d", setupNum); \
  assertIntSufHex(A, B, s); \
}

Junk * pJunk;
JunkIx iJunk;

#define CHOMPNOTHING 1
int doWhat;
void expect(int what) {doWhat |= what;}
int fullChompN; Score fullChompP[10];
void expectFullChomp(int n, Score * p) { fullChompN=n; memcpy(fullChompP, p, n*sizeof(Score)); }

bool setupEmpty(void) { meapOfJunks_open(); return true; }

bool setupSingleton(void) {
  setupEmpty();
  meapOfJunks_insert(0x88, 1, &iJunk);
  expect(CHOMPNOTHING);
  expectFullChomp(1, (Score []){0x88});
  return true;
}

bool setup2Inc(void) {
  setupSingleton();
  meapOfJunks_insert(0xc8, 2, &iJunk);
  expect(CHOMPNOTHING);
  expectFullChomp(2, (Score []){0x88, 0xc8});
  return true;
}
bool setup2Dec(void) {
  setupSingleton();
  meapOfJunks_insert(0x48, 3, &iJunk);
  expect(CHOMPNOTHING);
  expectFullChomp(2, (Score []){0x48, 0x88});
  return true;
}
bool setup2CloseInc(void) {
  setupSingleton();
  meapOfJunks_insert(0x89, 4, &iJunk);
  expect(CHOMPNOTHING);
  expectFullChomp(2, (Score []){0x88, 0x89});
  return true;
}
bool setup2CloseDec(void) {
  setupSingleton();
  meapOfJunks_insert(0x87, 5, &iJunk);
  expectFullChomp(2, (Score []){0x88, 0x87}); // Cos score (/16) is same
  expect(CHOMPNOTHING);
  return true;
}

bool setup3(Score a, Score b, Score c) {
  setupEmpty();
  meapOfJunks_insert(a, 6, &iJunk);
  meapOfJunks_insert(b, 7, &iJunk);
  meapOfJunks_insert(c, 8, &iJunk);
  expectFullChomp(3, (Score []){0x18, 0x28, 0x38});
  return true;
}

bool setup123(void)   { return setup3(0x18,0x28,0x38) || true; }
bool setup132(void)   { return setup3(0x18,0x38,0x28) || true; }
bool setup213(void)   { return setup3(0x28,0x18,0x38) || true; }
bool setup231(void)   { return setup3(0x28,0x38,0x18) || true; }
bool setup312(void)   { return setup3(0x38,0x18,0x28) || true; }
bool setup321(void)   { return setup3(0x38,0x28,0x18) || true; }
bool setup3Same(void) {
  setup3(0x37,0x38,0x39);
  expect(CHOMPNOTHING);
  expectFullChomp(3, (Score []){0x37,0x39,0x38}); // Shouldn't be fussy about the order
  return true;
}


B setterUppers[] = {
  setupEmpty, setupSingleton,
  setup2Inc, setup2Dec, setup2CloseInc, setup2CloseDec,
  setup123, setup132, setup213, setup231, setup312, setup321, setup3Same
};

#define numMeapSetups (sizeof(setterUppers)/sizeof(B))

int setupNum, testNum;

int counts[numMeapSetups ] = {0,1,2,2,2,2,3,3,3,3,3,3,3};
bool howMany(int killed) {
  Ix cnt = meapOfJunks_size();
  Ix exp = counts[setupNum]-killed;
  assertIntM(cnt, exp);
  return true;
}

bool ordered(void) {
  bool b = meapOfJunks_check();
  assertInt(b, true);
  return b;
}

bool testMeap1(void) {
  return
    howMany(0) &&
    ordered() &&
    true;
}

bool testMeap2(void) {
  return
    howMany(0) &&
    true;
}

bool testMeap3(void) {
  return
    (meapOfJunks_erase((JunkIx){2})) &&
    howMany(setupNum>0 ? 1 : 0) &&
    ordered() &&
    true;
}


bool testMeap4(void) {
  Ix cnt = meapOfJunks_size();
  if (cnt==0) return true;
  for (int a=0;a<500;a++) {
    JunkIx i = (JunkIx) {a%cnt};
    meapOfJunks_editTocks(i,rand()%0x100);
    ordered();
  }
  return true;
}

//bool testMeap5(void) { }

void cleanupMeap(void) { meapOfJunks_close(Hide); }

bool chompT(void) {
  Junk j;
  Chomped res;
  setupEmpty();
  j = (Junk){0}; res = meapOfJunks_chomp(8, &j, 0);
  assertIntM(res, Extinct);
  cleanupMeap();
  setup123();
  meapOfJunks_show();
  j = (Junk){0}; res = meapOfJunks_chomp(8, &j, 0);
  assertIntM(res, Idle); assertIntM(j.tocks, 0x18);
  j = (Junk){0}; res = meapOfJunks_chomp(0x18, &j, 0);
  assertIntM(res, Killed); assertIntM(j.tocks, 0x18);
  j = (Junk){0}; res = meapOfJunks_chomp(0x18, &j, 0);
  assertIntM(res, Idle); assertIntM(j.tocks, 0x28);
  j = (Junk){0}; res = meapOfJunks_chomp(0x98, &j, 0);
  assertIntM(res, Killed); assertIntM(j.tocks, 0x28);
  j = (Junk){0}; res = meapOfJunks_chomp(0x98, &j, 0);
  assertIntM(res, Killed); assertIntM(j.tocks, 0x38);
  j = (Junk){0}; res = meapOfJunks_chomp(0x98, &j, 0);
  assertIntM(res, Extinct); assertIntM(j.tocks, 0);
  meapOfJunks_show();
  return true;
}

bool chompTest(void) {
  return bkt("chompTest",nowt, chompT, cleanupMeap);
}

B testers[] = {testMeap1, testMeap2, testMeap3, testMeap4}; //, testMeap3, testMeap4};
#define numMeapTesters (sizeof(testers)/sizeof(B))


bool meap(void) {
  if (! chompTest()) return false;
  for (testNum=0;testNum<numMeapTesters;testNum++) {
    for (setupNum=0;setupNum<numMeapSetups; setupNum++) {
      doWhat=0; fullChompN=0;
      char blah[20];
      sprintf(blah, "meap[t=%d,s=%d]", testNum, setupNum);
      bkt(blah,setterUppers[setupNum], testers[testNum], cleanupMeap);
    }
  }
  return true;
}

void showJunk(JunkIx i, Junk * pJunk) {
  printf("tocks=0x%x\n", pJunk->tocks);
}

bool onJunkMeapWillErase(JunkIx i, Junk * p) { 
  return true; // Don't inhibit deleting anything.
}

