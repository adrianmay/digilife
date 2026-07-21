
#define SIZE_MOB ((double) hotelOfMobs_recBoth()) 
#define SIZE_MSG ((double) raffleOfMsgs_rec()) 
#define SIZE_BOTH (SIZE_MOB+SIZE_MSG)  
#define MOB_PROP (SIZE_MOB/SIZE_BOTH)  
#define MSG_PROP (SIZE_MSG/SIZE_BOTH)  
#define DOLE 1000000
#define SPAWN_COST 5000000
#define MURDER_RATE 20
#define CYCLES_PER_JOB GUESS_CYCLES_PER_TOCK

void seed(int n, Cash c, Cash thresh);
bool draw(void);
void onTockCore();
Cash runInCore(Cash cash, Mob * pMob, Msg * pMsg, char * out, int outlen);


#define _0 "\x00"
#define _end "\x04"
#define _snd "\x0F"
#define _print0 "\x2C"
#define _rollCash "\x09"
