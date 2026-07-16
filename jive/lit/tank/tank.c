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



void onMobHotel_goDie(MobIx i, Mob * pT) {
}
void onMobHotel_rentCollected (Cash rent) {}
void onMobHotel_rentDefaulted (Cash rent) {}
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
    p->_.test.spawnThresh = thresh; 
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

Cash run(Msg * pMsg, Mob * pMob, Cash cash) {
  cash += DOLE;
  if (cash > pMob->_.test.spawnThresh) {
    Cash childCash = cash/2;
    cash -= childCash;
    spawn(childCash, pMob->_.test.spawnThresh);
  }
  void stuffMsg(Msg * pNewMsg) {
    memcpy(pNewMsg, pMsg, sizeof(*pMsg)); 
  }
  raffleOfMsgs_play(cash*MSG_PROP, 100, stuffMsg); 
  cash -= cash*MSG_PROP;
  hotelOfMobs_drop(pMsg->rcvr.i, cash);
  notifyCycles(CYCLES_PER_JOB);
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
  run(pMsg, pMob, msgCash + mobCash);
  return 0; 
  hotelOfMobs_raid();
}

void onTockTank() {}

