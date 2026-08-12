#include <string.h>
#include "types.h"
#include "globals/api.h"
#include "misc/api.h"
#include "Mob.h"
#include "Msg.h"
#include "Mob_hotel/api.h"
#include "Msg_raffle/ix.h"
#include "Msg_raffle/api.h"
#include "api.h"

TockPrice totRent() { return hotelOfMobs_rent() + raffleOfMsgs_rent(); }

void onMobHotel_goDie(MobIx i, Mob * pT) { }
void onMobHotel_rentCollected (Cash rent) {}
void onMobHotel_rentDefaulted (Cash rent) { printf("Mob rent defaulted: %'ld\n", rent); }
void onMobHotel_extinct       (void) { raffleOfMsgs_quit(); }
void onMobHotel_funeral(MobIx, Mob * pMob) {}

void onMsgRaffle_extinct() { raffleOfMsgs_quit();  } // Not when we have external msg sources

void showMob(MobIx i, Mob * p) {
  switch (p->phylum) { 
    case PhyGod:
      break;
    case PhyMortal:
      MortalMob * pMortalMob = &p->_.mortal; 
      printf("spawnThresh=%ld code=", pMortalMob->spawnThresh);
      for (int i=0;i<sizeof(Program);i++) printf("%.2X ", pMortalMob->program[i]);
      break;
    default:
      DIE("Unknown phylum: %d\n", p->phylum);
  }
}
  
void showMsg(MsgIx i, Msg * p) {
  char r[50], s[50];
  hotelOfMobs_showsTact(r, p->rcvr);
  hotelOfMobs_showsTact(s, p->sndr);
  printf("bid=%.3f %s %s", p->cpuBid, r, s);
}
  

void dumpPiles(void) {
  printf("\n");
  hotelOfMobs_show();
  printf("\n");
  raffleOfMsgs_show();
  printf("\n");
}

#define SAMPLER(NAME, SPEED) \
  float NAME##Mean; \
  int NAME##Samples=0; \
  void NAME##Sample(float val) { \
    if (iterations < 1000000) return; \
    NAME##Samples++; \
    float speed = MAX(SPEED, 1.0/NAME##Samples); \
    NAME##Mean = speed*val + (1.0-speed)*NAME##Mean; \
  }

SAMPLER(msgcash,   0.000000001)
SAMPLER(mobcash,   0.000000001)
SAMPLER(childcash, 0.000000001)
SAMPLER(thresh,    0.000000001)
SAMPLER(pop,       0.000000001)
SAMPLER(spawned,   0.000000001)

#define HISTOGRAM(NAME, BUCKETS, BOT, STEP) \
  int NAME##Buckets[BUCKETS]={0};  \
  void NAME##Plop(float val) { int b = (val - BOT) / STEP; NAME##Buckets[b]++; } \
  void NAME##Hist##Show() { for (int a=0;a<BUCKETS;a++) \
    { printf("%.0f-%.0f : %d\n", BOT+STEP*a, BOT+STEP*(a+1), NAME##Buckets[a]); }}
  
HISTOGRAM(spare, 30, -1000000.0, 100000.0)

void onTockCore() {}

bool draw() { return raffleOfMsgs_draw(); }

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct Core {
  Cash cash;
  MobTact tMob;
  Mob * pMob;
  Msg * pMsg;
  int ip; // Instruction pointer
  char * out;
  int outlen;
  int outcur;
} Core;

// Returns change of scope depth
// doit is false when skipping 'then' or 'else' blocks.
typedef int (*Instruction)(Core *, bool doit);

typedef struct Op {
  char name[10];
  Instruction inst;
} Op;

Op ops[256];
int doBlock(Core * pC, bool doit);

void incIP(Core * pC, int n) { pC->ip += n; } // Overrun protection is in getInstruction below

int nop0(Core * pC, bool doit) { incIP(pC, 1); return 0; }
int nopn(Core * pC, bool doit) { return nop0(pC, doit); } //TODO: chomp n, then n bytes
int end   (Core * pC, bool doit) { incIP(pC, 1); return -1; }
int snd   (Core * pC, bool doit) { incIP(pC, 1); doit = !doit; return -1; }

int post_(MobTact rcvr, Cash cash, Core * pC, bool doit) {
  incIP(pC, 1);
  if (!doit) return 0;
  Cash bill = POST_COST + cash;
  if (pC->cash < bill) return 0;
  pC->cash -= bill;
  void stuffMsg(Msg * p) { p->cpuBid = 0; p->sndr = pC->tMob; p->rcvr = rcvr; }
  raffleOfMsgs_play(cash, 100, stuffMsg); 
  return 0; 
}
int post0(Core * pC, bool doit) { return post_(pC->tMob, pC->cash*MSG_PROP, pC, doit); }
int post1(Core * pC, bool doit) { return post0(pC, doit); }
int post2(Core * pC, bool doit) { return post0(pC, doit); }
int post3(Core * pC, bool doit) { return post0(pC, doit); }

int spawn0(Core * pC, bool doit) { 
  incIP(pC, 1);
  if (!doit) return 0;
  pC->cash -= SPAWN_COST;
  //printf("A: %'ld\n", pC->cash);
  Cash childCash = pC->cash/2;
  pC->cash -= childCash;
  Cash chMobCash = childCash * MOB_PROP;
  Cash chMsgCash = childCash - chMobCash;
  void stuffMob(Mob * p) { memcpy(p, pC->pMob, sizeof(Mob)); }
  MobTact tNewMob = hotelOfMobs_admit(chMobCash, false, stuffMob, 0, 0);
  void stuffMsg(Msg * p) { p->cpuBid = 0; p->sndr = pC->tMob; p->rcvr = tNewMob; }
  raffleOfMsgs_play(chMsgCash, 100, stuffMsg); 
  return 0; 
}
int spawn1(Core * pC, bool doit) { return spawn0(pC, doit); }
int spawn2(Core * pC, bool doit) { return spawn0(pC, doit); }
int spawn3(Core * pC, bool doit) { return spawn0(pC, doit); }

uint8_t * getRawOpCodeP(Core * pC) { 
  uint8_t * pI = &pC->pMob->_.mortal.program[pC->ip];
  return pI; 
}

Instruction getInstruction(Core * pC, bool doit) { // doit just for debugging
  sleepNs(1000000); // So I can hit Ctrl-C
  if (pC->ip < sizeof(Program)) {
    //printf("getInstruction with ip=%d and doit=%b returning: %s\n", pC->ip, doit, ops[*getRawOpCodeP(pC)].name);
    return ops[*getRawOpCodeP(pC)].inst; 
  }
  else {
    printf("getInstruction with ip=%d returning auto end\n", pC->ip);
    return end; // TODO: Should apply overrun fine here
  }
}

int print0(Core * pC, bool doit) { 
  incIP(pC, 1);
  int len = strlen((char*)getRawOpCodeP(pC));
  if (doit) {
    //printf("Print %s\n", (char*)getRawOpCodeP(pC));
    int n = snprintf(pC->out+pC->outcur, pC->outlen-pC->outcur, "%s", getRawOpCodeP(pC));
    pC->outcur += n;
  }
  incIP(pC, len+1); // Terminator
  return 0; 
}

int doBlock(Core * pC, bool doit) { // false means skip
  int level = 1;                                  
  while (level>0) { level += getInstruction(pC, doit)(pC, doit); }
  return -1;
}

bool doThen(Core * pC, bool doit) { // return whether ended with snd
  int level = 1;                                  
  Instruction inst;
  while (level>0) { 
    inst = getInstruction(pC, doit);
    level += inst(pC, doit); 
  }
  return (inst == snd);
}

int roll_(float x, Core * pC, bool doit) {
  if (!doit) {
    incIP(pC, 1 + 2*sizeof(float)); 
    if (doThen(pC, false)) doThen(pC, false);
  } else {
    incIP(pC, 1);
    float mu, amgis;  // Inverse of sigma, -ve for if (!...)
    memcpy((char*)&mu, (char*)&pC->pMob->_.mortal.program[pC->ip], sizeof(mu));
    incIP(pC, sizeof(float));
    memcpy((char*)&amgis, (char*)&pC->pMob->_.mortal.program[pC->ip], sizeof(amgis));
    incIP(pC, sizeof(float));
    bool lucky = rollCumGauss(x, mu, amgis);
    //printf("Rolled: %b\n", lucky);
    if (doThen(pC, lucky)) doThen(pC, !lucky);
  }
  return 0;

}
int rollCash(Core * pC, bool doit) { return roll_(pC->cash, pC, doit); }
int roll0(Core * pC, bool doit) { return roll_(0, pC, doit); }

Cash runInCore(Cash cash, MobTact tMob, Mob * pMob, Msg * pMsg) {
  memset(out, 0, outlen);
  Core core = (Core){cash, tMob, pMob, pMsg, 0, out, outlen, 0};
  doBlock(&core, true);
  return core.cash;
}

Cash run(MobTact tMob, Mob * pMob, Msg * pMsg, Cash mobCash, Cash msgCash) {
  Cash cash = msgCash + mobCash;
  msgcashSample(msgCash);
  mobcashSample(mobCash);
  cash += DOLE;
  notifyCycles(CYCLES_PER_JOB);
  cash -= totRent(); // Cos both msg and mob will miss out on the tock we expend in here
  cash = runInCore(cash, tMob, pMob, pMsg);
  hotelOfMobs_drop(pMsg->rcvr.i, cash);
  threshSample(pMob->_.mortal.spawnThresh);
  popSample(hotelOfMobs_count());
  if (iterations < 1000 || iterations % 100000 == 0) 
    printf("Its=%'ld, Rent=%'.0f, thresh=%'.0f; Means: pop=%'.2f, spawnOdds=%'.5f, childCash=%'.0f msgCash=%'.0f, mobCash=%'.0f, totCash=%'.0f\n",
        iterations, totRent(), threshMean, popMean, 1.0/spawnedMean, childcashMean, msgcashMean, mobcashMean, msgcashMean+mobcashMean);
  return cash;
}

Cash onMsgRaffle_dispatch(MsgTicketTact t, Msg * pMsg, Cash msgCash, V claim, V unlock) {
  printf("Raffle dispatch msg %d\n", t.i.i);
  Mob * pMob;
  Cash mobCash;
  Woth w = hotelOfMobs_grab(&pMsg->rcvr, &pMob, &mobCash);
  if (w==Dead) { unlock(); return 0; }       // Bankrupt msg
  if (w==Busy) { unlock(); return msgCash; } // Leave msg alone
  // So we got it
  claim();
  unlock();
  if (randIntBelow(MURDER_RATE)==0) 
    hotelOfMobs_drop(pMsg->rcvr.i, 0);
  else
    run(pMsg->rcvr, pMob, pMsg, mobCash, msgCash);
  return 0; 
}

void create(Cash c, Cash thresh) {
  void stuff(Mob * p) { 
    p->phylum = PhyMortal;
    char code[] = _spawn0 _post0 _end;
    memcpy((char*)p->_.mortal.program, code, sizeof(code));
    //Cash vm = thresh*0.03;
    //Cash vd = (randIntBelow(5)-2)*vm; 
    p->_.mortal.spawnThresh = thresh; // + vd; 
  }
  MobTact tNewMob = hotelOfMobs_admit(c*MOB_PROP, false, stuff, 0, 0);
  void stuffMsg(Msg * p) { p->cpuBid = 0; p->sndr = p->rcvr = tNewMob; }
  raffleOfMsgs_play(c*MSG_PROP, 100, stuffMsg); 
}

void seed(int n, Cash c, Cash thresh) {
  //hotelOfMobs_admit(0, true, 0, 0, 0);
  for (int a=0;a<n;a++) create(c, thresh);
}


#define _(NAME) { #NAME, NAME }

Op ops[256] = {
  _(nop0),   _(nopn),   _(nop0),   _(nopn),   /**/ _(end),    _(end),    _(end),    _(end),    /**/ _(roll0),  _(rollCash), _(roll0),  _(rollCash), /**/ _(end),     _(snd),   _(end),   _(snd),
  _(nop0),   _(nopn),   _(nop0),   _(nopn),   /**/ _(end),    _(end),    _(end),    _(end),    /**/ _(roll0),  _(rollCash), _(roll0),  _(rollCash), /**/ _(end),     _(snd),   _(end),   _(snd),
  _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(post0), _(post1),    _(post2), _(post3),    /**/ _(print0),  _(post1), _(post2), _(post3),
  _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(post0), _(post1),    _(post2), _(post3),    /**/ _(post0),   _(post1), _(post2), _(post3),
  _(nop0),   _(nopn),   _(nop0),   _(nopn),   /**/ _(end),    _(end),    _(end),    _(end),    /**/ _(roll0),  _(rollCash), _(roll0),  _(rollCash), /**/ _(end),     _(snd),   _(end),   _(snd),
  _(nop0),   _(nopn),   _(nop0),   _(nopn),   /**/ _(end),    _(end),    _(end),    _(end),    /**/ _(roll0),  _(rollCash), _(roll0),  _(rollCash), /**/ _(end),     _(snd),   _(end),   _(snd),
  _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(post0), _(post1),    _(post2), _(post3),    /**/ _(post0),   _(post1), _(post2), _(post3),
  _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(post0), _(post1),    _(post2), _(post3),    /**/ _(post0),   _(post1), _(post2), _(post3),
  _(nop0),   _(nopn),   _(nop0),   _(nopn),   /**/ _(end),    _(end),    _(end),    _(end),    /**/ _(roll0),  _(rollCash), _(roll0),  _(rollCash), /**/ _(end),     _(snd),   _(end),   _(snd),
  _(nop0),   _(nopn),   _(nop0),   _(nopn),   /**/ _(end),    _(end),    _(end),    _(end),    /**/ _(roll0),  _(rollCash), _(roll0),  _(rollCash), /**/ _(end),     _(snd),   _(end),   _(snd),
  _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(post0), _(post1),    _(post2), _(post3),    /**/ _(post0),   _(post1), _(post2), _(post3),
  _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(post0), _(post1),    _(post2), _(post3),    /**/ _(post0),   _(post1), _(post2), _(post3),
  _(nop0),   _(nopn),   _(nop0),   _(nopn),   /**/ _(end),    _(end),    _(end),    _(end),    /**/ _(roll0),  _(rollCash), _(roll0),  _(rollCash), /**/ _(end),     _(snd),   _(end),   _(snd),
  _(nop0),   _(nopn),   _(nop0),   _(nopn),   /**/ _(end),    _(end),    _(end),    _(end),    /**/ _(roll0),  _(rollCash), _(roll0),  _(rollCash), /**/ _(end),     _(snd),   _(end),   _(snd),
  _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(post0), _(post1),    _(post2), _(post3),    /**/ _(post0),   _(post1), _(post2), _(post3),
  _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(spawn0), _(spawn1), _(spawn2), _(spawn3), /**/ _(post0), _(post1),    _(post2), _(post3),    /**/ _(post0),   _(post1), _(post2), _(post3),
}; 

