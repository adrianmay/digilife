#include "types.h"
#include "Mob.h"
#include "Msg.h"
#include "Mob_hotel/api.h"
#include "Msg_raffle/ix.h"
#include "Msg_raffle/api.h"

void onMobHotel_goDie(MobIx i, Mob * pT) {}
void onMobHotel_rentCollected (Cash rent) {}
void onMobHotel_rentDefaulted (Cash rent) {}
void onMobHotel_extinct       (void) {}

Cash onMsgRaffle_dispatch(Msg * pMsg, Cash cash, V claim, V unlock) { return 0; }
void onMsgRaffle_extinct() {  }

void showMob(MobIx i, Mob * pMob) {
  printf("spawnThresh=%ld\n", pMob->spawnThresh);
}
  
void showMsg(MsgIx i, Msg * pMsg) {
  //printf("spawnThresh=%ld\n", pMob->spawnThresh);
}
  
static bool shouldQuit = false;

void loop() {
  while (!shouldQuit) {

  }
}

