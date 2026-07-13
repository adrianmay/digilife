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
  printf("onMobHotel_goDie\n");
}
void onMobHotel_rentCollected (Cash rent) {}
void onMobHotel_rentDefaulted (Cash rent) {}
void onMobHotel_extinct       (void) { raffleOfMsgs_quit(); }

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

void loop() { 
  //raffleOfMsgs_show();
  do {
    printf("\n#####################################################\n\n");
    printf("Tocks=%d Mobs=%d Msgs=%d\n", tocksNow(), hotelOfMobs_count(), raffleOfMsgs_count()); 
    hotelOfMobs_show();
    printf("\n");
    raffleOfMsgs_show();
    printf("\n");
    printf("\n");
  } while (raffleOfMsgs_draw()); 
}

void onMsgRaffle_dispatch(MsgIx i, Msg * pMsg, Cash msgCash, V claim, V unlock, V_C drop) {
  printf("onMsgRaffle_dispatch: msgIx= %d pMsg->rcvr=%d\n", i.i, pMsg->rcvr.i.i);
  Mob * pMob;
  Cash mobCash;
  if ((pMob = hotelOfMobs_grab(pMsg->rcvr, &mobCash))) {
    mobCash += msgCash + DOLE;
    msgCash = 0;
    claim();
    unlock();
    if (mobCash > pMob->_.test.spawnThresh) {
      Cash childCash = mobCash/2;
      printf("onMsgRaffle_dispatch spawn:    mobCash was %ld, childCash is %ld\n", mobCash, childCash);
      mobCash -= childCash;
      spawn(childCash, pMob->_.test.spawnThresh);
    }
    void stuffMsg(Msg * pNewMsg) { memcpy(pNewMsg, pMsg, SIZE_MSG); }
    printf("onMsgRaffle_dispatch self-msg: mobCash=%ld\n", mobCash);
    raffleOfMsgs_play(mobCash*MSG_PROP, 100, stuffMsg); 
    mobCash -= mobCash*MSG_PROP;
    hotelOfMobs_drop(pMsg->rcvr.i, mobCash);
    drop(0); // Kill msg
    notifyCycles(CYCLES_PER_JOB);
  } else { 
    unlock();
    drop(msgCash); 
  }
  hotelOfMobs_raid();
}

void onTockTank() {}
