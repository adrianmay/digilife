// TODO
//   Lang:
//     Roll
//     Pray: post to god without nick
//     Peer list: cash as function of number, select randomly from
//     Linear random always takes both limits
//     Recent senders, rcvrs, children
//     Cash read always takes param for mob/msg balance
//     Polynomials
//     Mutation, eagerness to mutate
//     Read registers
//     Loop, break
//     Persistent registers
//     Incoming msg body processing
//     Ougoing msg body production
//   Letter guesser:  
//     bookie god per letter

#include <string.h>
#include <setjmp.h>
#include "types.h"
#include "globals/api.h"
#include "misc/api.h"
#include "Mob.h"
#include "Msg.h"
#include "Mob_hotel/api.h"
#include "Msg_raffle/ix.h"
#include "Msg_raffle/api.h"
#include "api.h"
#include "ops.h"

void onTockCore() {}
TockPrice totRent() { return hotelOfMobs_rent() + raffleOfMsgs_rent(); }
void onMobHotel_goDie(MobIx i, Mob * pT) { }
void onMobHotel_rentCollected (Cash rent) {}
void onMobHotel_rentDefaulted (Cash rent) { printf("Mob rent defaulted: %'ld\n", rent); }
void onMobHotel_extinct       (void) { raffleOfMsgs_quit(); }
void onMobHotel_funeral(MobIx, Mob * pMob) {}
void onMsgRaffle_extinct() { raffleOfMsgs_quit();  } // Not when we have external msg sources
bool draw() { return raffleOfMsgs_draw(); }

void showMob(MobIx i, Mob * p) {
  switch (p->phylum) { 
    case PhyGod:
      break;
    case PhyMortal:
      MortalMob * pMortalMob = &p->_.mortal; 
      printf("code=");
      int i, lastNonZero;
      for (i=0;i<sizeof(Program);i++) if (pMortalMob->program[i]) lastNonZero=i; 
      for (i=0;i<=lastNonZero+1;i++) printf("%.2X ", pMortalMob->program[i]);
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

//////////////////////////////////////////////////////////
///  SAMPLING ////////////////////////////////////////////
//////////////////////////////////////////////////////////

#define SAMPLER(NAME, SPEED) \
  float NAME##Mean; \
  int NAME##Samples=0; \
  void NAME##Sample(float val) { \
    if (iterations < 0) return; \
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

//////////////////////////////////////////////////////////
///  THE CORE  ///////////////////////////////////////////
//////////////////////////////////////////////////////////

typedef struct Core {
  Cycles cyclesLeft;
  Cash mobCash;
  MobTact tMob;
  Mob * pMob;
  Msg * pMsg;
  int IP; // Inst pointer
  MobTact tChild;
  uint8_t * pChildProg;
  int childIP; // Inst pointer
  char * out;
  int outlen;
  int outcur;
  jmp_buf jb;
} Core;

#include "ops.cc"

void incIP(Core * pC, int n) { pC->IP += n; }

void chargeCpuTime(Core * pC, Cycles cycles) {
  if (pC->cyclesLeft <= cycles) longjmp(pC->jb, 1); //TODO: fixme
  pC->cyclesLeft -= cycles;
}

void chargeMobCash(Core * pC, Cash cost) {
  if (pC->mobCash <= cost) longjmp(pC->jb, 2);
  pC->mobCash -= cost;
}

uint8_t * getRawOpCodeP(Core * pC) { 
  uint8_t * pI = &pC->pMob->_.mortal.program[pC->IP];
  return pI; 
}

uint8_t I(Core * pC, Doit doit) { // doit just for debugging
  sleepNs(10000); // So I can hit Ctrl-C
  if (pC->IP < sizeof(Program)) {
    //printf("I with ip=%d and doit=%d returning: %s/%s\n", pC->ip, doit, opnames[*getRawOpCodeP(pC)], testnames[*getRawOpCodeP(pC)]);
    return *getRawOpCodeP(pC); 
  } else {
    printf("I with ip=%d returning auto end\n", pC->IP);
    return __end; // TODO: Should apply overrun fine here
  }
}

//////////////////////////////////////////////////////////
/// INSTRUCTIONS   ///////////////////////////////////////
//////////////////////////////////////////////////////////

Cycles getInstCpuCycles(Core * pC, Doit doit) { return (doit==doingit) ? cpuCyclesOfInsts[I(pC, doit)] : 1; }

void doInst(Core * pC, Doit doit) { 
  uint8_t x = I(pC, doit);
  Inst * f = funcsForInsts[x]; 
  chargeCpuTime(pC, getInstCpuCycles(pC, doit));
  //if (doit==quiningit) quineInst(x, pC);
  incIP(pC, 1); 
  f(pC, doit); 
}

/////// FLOW CONTROL
void nop   (Core * pC, Doit doit) { doInst(pC, doit); } // Tail-recurse to next instruction
void end   (Core * pC, Doit doit) { } // Return to calling block or end of program
// New if block, maybe already passive cos of surrounding ifels stuff
Doit onIff  [3][2] = { { todoit, doingit } // doingit: Surroundings normal, do it if true, or allow future ifels
                     , { doneit, doneit  } // doneit:  Surroundings inhibitory, keep whole block inhibited
                     , { doneit, doneit  } // todoit:  Surroundings inhibitory, keep whole block inhibited
                     };
Doit onElsif[3][2] = { { doneit, doneit  } // doingit: Already doing something, this ifels is ruled out
                     , { doneit, doneit  } // doneit:  Already done it, also ruled out
                     , { todoit, doingit } // todoit:  Still looking for matching ifels, so do it or stay in same state 
};
void iff   (Core * pC, Doit doit) { bool b = doTest(pC, doit); doInst(pC, onIff[doit][b]); doInst(pC, doit); }
void elsif (Core * pC, Doit doit) { bool b = doTest(pC, doit); doInst(pC, onElsif[doit][b]); } // Fall back to the iff which does next instruction

/////// WORLD
void post(Core * pC, Doit doit) {
  MobTact rcvr = doPeer(pC, doit);
  float cash = doFloat(pC, doit);
  if (doit != doingit) return;
  chargeMobCash(pC, cash);
  void stuffMsg(Msg * p) { p->cpuBid = 0; p->sndr = pC->tMob; p->rcvr = rcvr; }
  raffleOfMsgs_play(cash, 100, stuffMsg); 
}

void quine(Core * pC, Mob * pChild) { memcpy(pChild, pC->pMob, sizeof(Mob)); }
void spawn(Core * pC, Doit doit) { 
  if (doit != doingit) return;
  //printf("Spawned: %'ld\n", pC->cash);
  Cash childCash = pC->mobCash/2;
  chargeMobCash(pC, childCash);
  Cash chMobCash = childCash * MOB_PROP;
  Cash chMsgCash = childCash - chMobCash;
  void stuffMob(Mob * p) { quine(pC, p); }
  pC->tChild = hotelOfMobs_admit(chMobCash, false, stuffMob, 0, 0);
  void stuffMsg(Msg * p) { p->cpuBid = 1; p->sndr = pC->tMob; p->rcvr = pC->tChild; }
  raffleOfMsgs_play(chMsgCash, 100, stuffMsg); 
}

/////// OUTPUT
void prs(Core * pC, Doit doit) { 
  int len = strlen((char*)getRawOpCodeP(pC));
  chargeCpuTime(pC, len);
  if (doit==doingit) {
    chargeCpuTime(pC, 3*len);
    //printf("Print %s\n", (char*)getRawOpCodeP(pC));
    int n = snprintf(pC->out+pC->outcur, pC->outlen-pC->outcur, "%s", getRawOpCodeP(pC));
    pC->outcur += n;
  }
  incIP(pC, len+1); // Terminator
  doInst(pC, doit);
}

void prf(Core * pC, Doit doit) { 
  float f = doFloat(pC, doit);
  if (doit==doingit) {
    int n = snprintf(pC->out+pC->outcur, pC->outlen-pC->outcur, "%f", f);
    pC->outcur += n;
  }
  doInst(pC, doit);
}
 
void disas(Core * pC, Doit doit) { }

//////////////////////////////////////////////////////////
/// TESTS  ///////////////////////////////////////////////
//////////////////////////////////////////////////////////

Cash getTestCpuCycles(Core * pC, Doit doit) { return (doit==doingit) ? cpuCyclesOfTests[I(pC, doit)] : 1; }

bool doTest(Core * pC, Doit doit) {
  uint8_t x = I(pC, doit);
  chargeCpuTime(pC, getTestCpuCycles(pC, doit));
  //if (doit==quiningit) quineTest(x, pC);
  incIP(pC, 1);
  return funcsForTests[x](pC, doit);
}

bool yes   (Core * pC, Doit doit) { return true;  }
bool no    (Core * pC, Doit doit) { return false; }
bool not   (Core * pC, Doit doit) { return !doTest(pC, doit); }
bool gt    (Core * pC, Doit doit) { 
  float a = doFloat(pC, doit); float b = doFloat(pC, doit); return b > a; } // Ordering deliberate
bool like  (Core * pC, Doit doit) {
  float a = doFloat(pC, doit); float b = doFloat(pC, doit); float c = doFloat(pC, doit); 
  return abs(c-b) <= a; }

//////////////////////////////////////////////////////////
/// FLOATS  //////////////////////////////////////////////
//////////////////////////////////////////////////////////

Cash getFloatCpuCycles(Core * pC, Doit doit) { return (doit==doingit) ? cpuCyclesOfFloats[I(pC, doit)] : 1; }

float doFloat(Core * pC, Doit doit) {
  uint8_t x = I(pC, doit);
  chargeCpuTime(pC, getFloatCpuCycles(pC, doit));
  //if (doit==quiningit) quineFloat(x, pC);
  incIP(pC, 1);
  return funcsForFloats[x](pC, doit);
}

/////// CONSTANTS
float zero(Core * pC, Doit doit) { return 0; }
float one (Core * pC, Doit doit) { return 1; }
float two (Core * pC, Doit doit) { return 2; }

/////// IMMEDIATE
float imm (Core * pC, Doit doit) { 
  float f;  // Inverse of sigma, -ve for if (!...)
  memcpy((char*)&f, (char*)&pC->pMob->_.mortal.program[pC->IP], sizeof(f));
  incIP(pC, sizeof(float));
  return f; }

/////// READ REGISTERS  
float csh(Core * pC, Doit doit) { return (float) pC->mobCash; }
float cyc(Core * pC, Doit doit) { return (float) pC->cyclesLeft; }
float reg  (Core * pC, Doit doit) { return 0; }

/////// RANDOM
float rndl (Core * pC, Doit doit) { return randFloatWithin(doFloat(pC, doit), doFloat(pC, doit)); }
float rndg (Core * pC, Doit doit) { return randGaussian(doFloat(pC, doit), doFloat(pC, doit)); }

/////// ARITHMETIC
float neg  (Core * pC, Doit doit) { return   0 - doFloat(pC, doit); }
float inv  (Core * pC, Doit doit) { return 1.0 / doFloat(pC, doit); }
float doBinop (Core * pC, Doit doit, float bop(float, float)) { 
  float a = doFloat(pC, doit); float b = doFloat(pC, doit); return bop(a, b); }
float add  (Core * pC, Doit doit) { float op(float a, float b) {return a+b;} return doBinop(pC, doit, op); }
float mul  (Core * pC, Doit doit) { float op(float a, float b) {return a*b;} return doBinop(pC, doit, op); }

//////////////////////////////////////////////////////////
/// PEERS  ///////////////////////////////////////////////
//////////////////////////////////////////////////////////

Cash getPeerCpuCycles(Core * pC, Doit doit) { return (doit==doingit) ? cpuCyclesOfPeers[I(pC, doit)] : 1; }

MobTact doPeer(Core * pC, Doit doit) {
  uint8_t x = I(pC, doit);
  chargeCpuTime(pC, getPeerCpuCycles(pC, doit));
  //if (doit==quiningit) quinePeer(x, pC);
  incIP(pC, 1);
  return funcsForPeers[x](pC, doit);
}

/////// READ REGISTERS
MobTact me     (Core * pC, Doit doit) { return pC->tMob; }
MobTact sndr   (Core * pC, Doit doit) { return pC->pMsg->sndr; }
MobTact child  (Core * pC, Doit doit) { return pC->tChild; }

/////// GODS
MobTact peer0  (Core * pC, Doit doit) { return (MobTact){(MobIx){0},0}; }
MobTact peer1  (Core * pC, Doit doit) { return (MobTact){(MobIx){1},0}; }
MobTact peer2  (Core * pC, Doit doit) { return (MobTact){(MobIx){2},0}; }
MobTact peer3  (Core * pC, Doit doit) { return (MobTact){(MobIx){3},0}; }
//MobTact rndpeer(Core * pC, Doit doit) { return (MobTact){(MobIx){0},0}; }

//////////////////////////////////////////////////////////
/// RUNNING   ///////////////////////////////////////
//////////////////////////////////////////////////////////

Cash runInCore(Cash mobCash, Cash msgCash, MobTact tMob, Mob * pMob, Msg * pMsg) {
  memset(out, 0, outlen);
  Cycles cyc = msgCash / pMsg->cpuBid; 
  Core core = (Core){cyc, mobCash, tMob, pMob, pMsg, 0, tMob, 0, 0, out, outlen, 0};
  if (0==setjmp(core.jb)) doInst(&core, doingit); 
  else ; //Ran out of msgCash
  return core.mobCash + core.cyclesLeft * pMsg->cpuBid;
}

void run(MobTact tMob, Mob * pMob, Msg * pMsg, Cash mobCash, Cash msgCash) {
  msgcashSample(msgCash);
  mobcashSample(mobCash);
  mobCash += DOLE;
  mobCash -= totRent(); // Cos both msg and mob will miss out on the tock we expend in here
  Cash finalCash = runInCore(mobCash, msgCash, tMob, pMob, pMsg);
  hotelOfMobs_drop(pMsg->rcvr.i, finalCash);
  Program * pProg = &pMob->_.mortal.program;          
  void * pVoid = (void *) pProg;
  float * pThresh = (float*) (pVoid+1);
  threshSample(*pThresh);
  popSample(hotelOfMobs_count());
  if (iterations < 1000 || iterations % 1000 == 0) {
    printf("Its=%'ld, Rent=%'.0f, threshMean=%'.0f; Means: pop=%'.2f, spawnOdds=%'.5f, childCash=%'.0f msgCash=%'.0f, mobCash=%'.0f, totCash=%'.0f\n",
        iterations, totRent(), threshMean, popMean, 1.0/spawnedMean, childcashMean, msgcashMean, mobcashMean, msgcashMean+mobcashMean);
  }
}

Cash onMsgRaffle_dispatch(MsgTicketTact t, Msg * pMsg, Cash msgCash, V claim, V unlock) {
  //printf("Raffle dispatch msg %d\n", t.i.i);
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

void create(Cash c, ProgStuffer stuffProg) {
  void stuffMob(Mob * p) { 
    p->phylum = PhyMortal;
    stuffProg(&p->_.mortal.program);
    //memcpy((char*)p->_.mortal.program, code, sizeof(code));
  }
  MobTact tNewMob = hotelOfMobs_admit(c*MOB_PROP, false, stuffMob, 0, 0);
  void stuffMsg(Msg * p) { p->cpuBid = 0; p->sndr = p->rcvr = tNewMob; }
  raffleOfMsgs_play(c*MSG_PROP, 100, stuffMsg); 
}

void seed(int n, Cash c, ProgStuffer stuffProg) {
  //hotelOfMobs_admit(0, true, 0, 0, 0);
  for (int a=0;a<n;a++) create(c, stuffProg);
}

// void Q(Core * pC)  { pC->pChildProg[pC->childIP++] = I(pC, quiningit); }
// void quineInst(uint8_t inst, Core * pC) { quinersForInsts[inst](pC); }
// void nopQ    (Core * pC) { Q(pC); }
// void endQ    (Core * pC) { Q(pC); }
// void iffQ    (Core * pC) { Q(pC); }
// void elsifQ  (Core * pC) { Q(pC); }
// void   sQ    (Core * pC) { pC->childIP += 1+strcpy_len(pC->pChildProg+pC->childIP, getRawOpCodeP(pC)); }
// void prsQ    (Core * pC) { Q(pC); sQ(pC); }
// void disasQ  (Core * pC) { Q(pC); } // TODO: ?
// void prfQ    (Core * pC) { Q(pC); }
// void postQ   (Core * pC) { Q(pC); }
// void spawnQ  (Core * pC) { Q(pC); }
// 
// void quineTest(uint8_t x, Core * pC) { quinersForTests[x](pC); }
// void noQ     (Core * pC) { Q(pC); }  
// void yesQ    (Core * pC) { Q(pC); }   
// void likeQ   (Core * pC) { Q(pC); }    
// void gtQ     (Core * pC) { Q(pC); }  
// void notQ    (Core * pC) { Q(pC); }   
// 
// void quineFloat(uint8_t x, Core * pC) { quinersForFloats[x](pC); }
// void zeroQ   (Core * pC) { Q(pC); }         
// void oneQ    (Core * pC) { Q(pC); }         
// void twoQ    (Core * pC) { Q(pC); }         
// void immQ    (Core * pC) { Q(pC); }         
// void cshQ    (Core * pC) { Q(pC); }         
// void cycQ    (Core * pC) { Q(pC); }         
// void rndlQ   (Core * pC) { Q(pC); }         
// void rndgQ   (Core * pC) { Q(pC); }         
// void addQ    (Core * pC) { Q(pC); }         
// void mulQ    (Core * pC) { Q(pC); }         
// void invQ    (Core * pC) { Q(pC); }         
// void negQ    (Core * pC) { Q(pC); }         
// 
// void quinePeer(uint8_t x, Core * pC) { quinersForPeers[x](pC); }
// void meQ     (Core * pC) { Q(pC); }               
// void sndrQ   (Core * pC) { Q(pC); }               
// void childQ  (Core * pC) { Q(pC); }                
// void peer0Q  (Core * pC) { Q(pC); }                
// void peer1Q  (Core * pC) { Q(pC); }                
// void peer2Q  (Core * pC) { Q(pC); }                
// void peer3Q  (Core * pC) { Q(pC); }                
//            
// void quine(Core * pC, Mob * pChild) {
//   int saveIP = pC->IP;
//   pC->pChildProg = (uint8_t*)&pChild->_.mortal.program;
//   pC->childIP = 0;
//   doInst(pC, quiningit);
//   pC->IP = saveIP;
  
  // In general: support shrinkage too
  // Insts: 
  //   Add preceding nop, post, spawn, print to post body, etc
  //   Add surrounding iff or elsif
  //   Simplify rarely used iff cases 
  // Floats:  
  //   Vary immediates
  //   Vary random distribution type
  //   Insert *1 or +0
  //   Insert * reg / typical value, etc
  //   Insert polynomial
  //   Simplify/remove polynomial with small coefficients
  //   Change cash type proportion 
  //   Insert random multiplier/shifter
  //   Remove factors if random element very high
  // Tests:
  //   Not much
  // Peers, peer list: 
  //   Grow/shrink
  //   Mutate to random live mortal
  //   Recent senders, rcvrs, children
  //   Gods
// }

