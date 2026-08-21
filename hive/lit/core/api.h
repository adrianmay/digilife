
#define SIZE_MOB ((float) hotelOfMobs_recBoth()) 
#define SIZE_MSG ((float) raffleOfMsgs_rec()) 
#define SIZE_BOTH (SIZE_MOB+SIZE_MSG)  
#define MOB_PROP (SIZE_MOB/SIZE_BOTH)  
#define MSG_PROP (SIZE_MSG/SIZE_BOTH)  
#define DOLE 1000000
#define SPAWN_COST 500000
#define POST_COST 50000
#define MURDER_RATE 20
#define MUTE_RATE 0.3
#define CYCLES_PER_JOB GUESS_CYCLES_PER_TOCK

typedef void (*ProgStuffer)(Program * pProg);
void seed(int n, Cash c, ProgStuffer stuffProg);
bool draw(void);
void onTockCore();
Cash runInCore(Cash mobCash, Cash msgCash, MobTact tMob, Mob * pMob, Msg * pMsg);
extern char out[];
extern int outlen;
struct Core;

#include "ops.h"
