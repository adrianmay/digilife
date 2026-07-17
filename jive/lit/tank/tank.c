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


TockPrice tankRent() { return hotelOfMobs_rent() + raffleOfMsgs_rent(); }

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
    case PhyTest:
      TestMob * pTestMob = &p->_.test; 
      printf("spawnThresh=%ld", pTestMob->spawnThresh);
      break;
    default:
      DIE("Unknown phylum\n");
  }
}
  
void showMsg(MsgIx i, Msg * p) {
  char r[50], s[50];
  hotelOfMobs_showsTact(r, p->rcvr);
  hotelOfMobs_showsTact(s, p->sndr);
  printf("bid=%.3f %s %s", p->cpuBid, r, s);
}
  
void spawn(Cash c, Cash thresh) {
  void stuff(Mob * p) { 
    p->phylum = PhyTest;
//    Cash vm = thresh*0.03;
//    Cash vd = (randIntBelow(5)-2)*vm; 
    p->_.test.spawnThresh = thresh; // + vd; 
  }
  MobTact tNewMob = hotelOfMobs_admit(c*MOB_PROP, false, stuff, 0, 0);
  void stuffMsg(Msg * p) { p->cpuBid = 0; p->sndr = p->rcvr = tNewMob; }
  raffleOfMsgs_play(c*MSG_PROP, 100, stuffMsg); 
}

void seed(int n, Cash c, Cash thresh) {
  hotelOfMobs_admit(0, true, 0, 0, 0);
  for (int a=0;a<n;a++) spawn(c, thresh);
}

bool draw() { return raffleOfMsgs_draw(); }

void dumpPiles(void) {
  printf("\n");
  hotelOfMobs_show();
  printf("\n");
  raffleOfMsgs_show();
  printf("\n");
}

//void loop() { 
//  //raffleOfMsgs_show();
//  do {
//    if (!raffleOfMsgs_check()) DIE("Raffle check failed\n");
////    printf("\n");
//    printf("Tocks=%d Mobs=%d Msgs=%d\n", tocksNow(), hotelOfMobs_count(), raffleOfMsgs_count()); 
////    printf("\n");
////    hotelOfMobs_show();
////    printf("\n");
////    raffleOfMsgs_show();
////    printf("\n");
////    printf("\n#####################################################\n\n");
//  } while (raffleOfMsgs_draw()); 
//}

#define SAMPLER(NAME, SPEED) \
  double NAME##Mean; \
  int NAME##Samples=0; \
  void NAME##Sample(double val) { \
    NAME##Samples++; \
    double speed = MAX(SPEED, 1.0/NAME##Samples); \
    NAME##Mean = speed*val + (1.0-speed)*NAME##Mean; \
  }

SAMPLER(msgcash, 0.00000001)
SAMPLER(mobcash, 0.00000001)
SAMPLER(childcash, 0.00000001)
SAMPLER(thresh, 0.0001)
SAMPLER(pop, 0.00000001)
SAMPLER(spawned, 0.00000001)

#define HISTOGRAM(NAME, BUCKETS, BOT, STEP) \
  int NAME##Buckets[BUCKETS]={0};  \
  void NAME##Plop(double val) { int b = (val - BOT) / STEP; NAME##Buckets[b]++; } \
  void NAME##Hist##Show() { for (int a=0;a<BUCKETS;a++) \
    { printf("%.0f-%.0f : %d\n", BOT+STEP*a, BOT+STEP*(a+1), NAME##Buckets[a]); }}
  
HISTOGRAM(spare, 30, -1000000.0, 100000.0)

Cash run(Msg * pMsg, Mob * pMob, Cash msgCash, Cash mobCash) {
  Cash cash = msgCash + mobCash;
  msgcashSample(msgCash);
  mobcashSample(mobCash);
  cash += DOLE;
  notifyCycles(CYCLES_PER_JOB);
  //cash -= tankRent(); // Cos both msg and mob will miss out on the tock we expend in here
  Cash spare = cash - pMob->_.test.spawnThresh;
  if (spare >= 0) {
    if (iterations > 1000000)
      sparePlop(spare);
    //if (iterations >= 20000000)
    //  spareHistShow();
    cash -= SPAWN_COST;
    Cash childCash = cash/2; // - spare/2;
    childcashSample(childCash);
    cash -= childCash;
    spawn(childCash, pMob->_.test.spawnThresh);
    spawnedSample(1);
  } else spawnedSample(0);
  void stuffMsg(Msg * pNewMsg) {
    memcpy(pNewMsg, pMsg, sizeof(*pMsg)); 
  }
  raffleOfMsgs_play(cash*MSG_PROP, 100, stuffMsg); 
  cash -= cash*MSG_PROP;
  hotelOfMobs_drop(pMsg->rcvr.i, cash);
  threshSample(pMob->_.test.spawnThresh);
  popSample(hotelOfMobs_count());
  if (iterations < 1000 || iterations % 100000 == 0) 
    printf("Its=%'ld, Rent=%'.0f Means: pop=%'.2f, childCash=%'.0f msgCash=%'.0f, mobCash=%'.0f, totCash=%'.0f, thresh=%'.0f, spawnOdds=%'.5f\n",
        iterations, tankRent(), popMean, childcashMean, msgcashMean, mobcashMean, msgcashMean+mobcashMean, threshMean, 1.0/spawnedMean);
  return cash;
}

Cash onMsgRaffle_dispatch(MsgIx i, Msg * pMsg, Cash msgCash, V claim, V unlock) {
  Mob * pMob;
  Cash mobCash;
  Woth w = hotelOfMobs_grab(pMsg->rcvr, &pMob, &mobCash);
  if (w==Dead) { unlock(); return 0; }       // Bankrupt msg
  if (w==Busy) { unlock(); return msgCash; } // Leave msg alone
  // So we got it
  claim();
  unlock();
  run(pMsg, pMob, msgCash, mobCash);
  return 0; 
  hotelOfMobs_raid();
}

void onTockTank() {}

