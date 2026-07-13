
#include "types.h"
#include "misc/api.h"
#include "globals/api.h"
#include "blob.h"
#include "bomb.h"
//#include "XXBomb_meap/ix.h"
//#include "XXBlob_pile/ix.h"
#include "XXBomb_meap/api.h"
#include "XXBlob_pile/api.h"
#include "ix.h"
#include "api.h"

// Assumes some mutex is held, despite the name
// That's true because onXXMeap_new only called from meap's insert
//   which for the hotel is only called from hotel's admit
// We know it exists, and it doesn't have or need money
void onXXBombMeap_new(XXBomb * pBomb, Ix hint) {
  pBomb->who = (XXBlobIx){hint};
//  XX * p = pileOfXXs_get(pBomb->who);
//  p->rent.bomb = iBomb;
//  updateDeathWithXXAndBomb_(p, pBomb);
//  meapOfXXBombs_check();
}

// Similarly thread safe already, I think?
void onXXBombMeap_move(XXBomb * pBomb, XXBombIx to) {
  XXBlob * pBlob = pileOfXXBlobs_get(pBomb->who);
  pBlob->rent.bomb = to;
  meapOfXXBombs_check();
}

bool onXXBombMeap_willErase(XXBombIx i, XXBomb * pBomb) {
  XXBlobIx who = pBomb->who;
  XXBlob * pBlob = pileOfXXBlobs_get(who);
  Nick was = atomic_fetch_or(&pBlob->rent.nick, NICK_FLAG_BOMBED);
  printf("BOMBING: onXXBombMeap_willErase %d was %08x\n", i.i, was);
  return (!(was & NICK_FLAG_BUSY)); // Comes later
}

int hotelOfXXs_showsTact(char * cursor, XXTact t) {
  return sprintf(cursor, "%-8x=%d", t.n, t.i.i);
}

void showXXBlob(XXBlobIx i, XXBlob * p) {
  char busy   = (p->rent.nick & NICK_FLAG_BUSY  ) ? 'U' : 'u';
  char bombed = (p->rent.nick & NICK_FLAG_BOMBED) ? 'O' : 'o';
  char god    = (p->rent.nick & NICK_NAME_GOD   ) ? 'G' : 'g';
  Ix base     = (p->rent.nick & NICK_NAME_RAND_MASK);
  printf("ix=%-4d nick=%c%c%c`%07x|lastPaidRent=%-5d cash=%-8ld bomb=%-2d ", i.i, busy, bombed, god, base, p->rent.lastPaidRent, p->rent.cash, p->rent.bomb.i);
  showXX((XXIx){i.i}, &p->body);
}

void showXXBomb(XXBombIx i, XXBomb * p) {
  printf("tocks=%d,who=%d", p->tocks, p->who.i);
}

void showXXPair(XXIx i, XX * p) {
  XXBlob * pBlob = pileOfXXBlobs_get((XXBlobIx){i.i});
  XXBomb * pBomb = meapOfXXBombs_get(pBlob->rent.bomb);
  showXXBlob((XXBlobIx){i.i} , pBlob);
  printf(" @ ");
  showXXBomb(pBlob->rent.bomb, pBomb);
}

void hotelOfXXs_show(void) {
  pileOfXXBlobs_show(false);
  printf("\n");
  meapOfXXBombs_show();
}

bool hotelOfXXs_open() {
  meapOfXXBombs_open();
  bool virgin = pileOfXXBlobs_open();
  return virgin;
}

Ix hotelOfXXs_count(void) { return pileOfXXBlobs_count(); }
Ix hotelOfXXs_top  (void) { return pileOfXXBlobs_top(); }

void hotelOfXXs_close(Fate fate) {
  pileOfXXBlobs_close(fate);
  meapOfXXBombs_close(fate);
}

double hotelOfXXs_rec(void) { return pileOfXXBlobs_rec() + meapOfXXBombs_rec(); }
TockPrice hotelOfXXs_rent() { return tockPrice() * hotelOfXXs_rec(); }

static void rebomb(XXRent * pRent, XXBlobIx i) {
  Tocks expiry = pRent->lastPaidRent + pRent->cash / hotelOfXXs_rent();
  meapOfXXBombs_insert(expiry, i.i, &pRent->bomb); // Do we need the return value?
}
 
XXTact hotelOfXXs_admit(Cash cash, bool isGod, V_XXP stuff, XX ** pp, bool * pRecycled) {
  if (cash == 0 && !isGod) 
    return (XXTact){(XXIx){BAD_INDEX}};
  XXBlob * pBlob;
  XXBlobIx iBlob = pileOfXXBlobs_alloc(&pBlob, pRecycled);
  pBlob->rent.bomb = badXXBombIx;
  pBlob->rent.cash = cash;
  pBlob->rent.lastPaidRent = tocksNow();
  Nick n = randInt32Masked(NICK_NAME_RAND_MASK) | ( isGod ? NICK_NAME_GOD : 0 );
  pBlob->rent.nick = n;
  if (stuff) stuff(&pBlob->body);
  XXTact t = (XXTact){(XXIx){iBlob.i}, n};
  if (isGod) { //God
    pileOfXXBlobs_modUsr(1); // This is the god count
  } else {
    rebomb(&pBlob->rent, iBlob);
  }
  if (pp) *pp = &pBlob->body;
  //printf("admit:nick=%d\n", p->rent.nick);
  return t;
}

static bool isGod(XXRent * pRent) { return pRent->nick & NICK_NAME_GOD ; }

void hotelOfXXs_collectRent(XXRent * pRent) {
  Cash collected, defaulted;
  Tocks now = tocksNow();
  Tocks timeUnpaid = now - pRent->lastPaidRent;
  pRent->lastPaidRent = now;
  Cash bill = hotelOfXXs_rent() * timeUnpaid;
  if (isGod(pRent) || pRent->cash >= bill) {
    pRent->cash -= bill;
    collected = bill;
    defaulted = 0;
  } else {
    Cash c = pRent->cash;
    pRent->cash = 0;
    collected = c;
    defaulted = bill-c;
  }
  if (collected) onXXHotel_rentCollected(collected);
  if (defaulted) onXXHotel_rentDefaulted(defaulted);
}

XX * hotelOfXXs_get(XXIx i) { 
  XXBlob * pBlob = pileOfXXBlobs_get((XXBlobIx){i.i});
  return &pBlob->body;
    
}

// GRAB AND RAID SYNCHRONISATION:

// When starting a job, the sum of the msg and mob cash is initialised in the core
//   without changing them in the msg/mob.
// Then money is received in subsidies and/or spent on actions in the mob code.
// After the job ends, the msg is left bankrupt and the mob keeps whatever money it should.
// At this point both bombs are talking rubbish. I considered deleting both bombs 
//   at the start of the job but this way is more efficient.
// A raid might have tried to destroy the mob or msg in the meantime but backed off
//   seeing them busy and marked them bombed. 
// drop puts everything to rights. It's not to be used unless a job just happened.

// AFAIK this is just for running a job
// If the XX is not busy, not doomed, and has the passed nick, mark it busy and return a pointer to it
XX * hotelOfXXs_grab(XXTact t, Cash * pCash) {
  XXBlob * pBlob = pileOfXXBlobs_get((XXBlobIx){t.i.i});
  Nick want, set; 
  want = t.n; set = t.n | NICK_FLAG_BUSY; 
  if (atomic_compare_exchange_strong(&pBlob->rent.nick, &want, set)) {
    hotelOfXXs_collectRent(&pBlob->rent);
    *pCash = pBlob->rent.cash;
    return &pBlob->body;
  }
  else {
    if (want == 0xFFFFFFFF) return 0;
    DIE("hotelOfXXs_grab: failed to grab %d\n", t.i.i);
    return 0;
  }
}
 
// As above if you don't know the nick cos you're the bomb.
// I don't want to bloat the bombs. 
// Since only raid removes XXs, I think I can assume the nick is correct.
XX * hotelOfXXs_grabIx(XXIx i, Cash * pCash) {
  XXBlob * pBlob = pileOfXXBlobs_get((XXBlobIx){i.i});
  XXTact t = (XXTact){i, pBlob->rent.nick};
  return hotelOfXXs_grab(t, pCash);
}

static bool updateDeathWithBomb(XXBlob * p, XXBombIx iBomb, XXBomb * pBomb) {
  Cash cash = p->rent.cash;
  Tocks ttl = cash / hotelOfXXs_rent();
  Tocks death = tocksNow() + ttl;
//  printf("updateDeathWithBomb: ttl=%d death=%d\n", ttl, death);
  meapOfXXBombs_check();
  // This changes bomb time and reorders meap:
  bool res = false;
  if (p->rent.bomb.i == iBomb.i) // Redundant if grabbed.
    res  = meapOfXXBombs_editTocks(p->rent.bomb, death); // Locks the meap itself
  meapOfXXBombs_check();
  return res;
  // The meap is now properly reordered but nobody called kill
}

static bool updateDeath(XXBlob * pBlob) {
  XXBombIx iBomb = pBlob->rent.bomb;
  XXBomb * pBomb = meapOfXXBombs_get(iBomb); // Assumes grabbed.
  return updateDeathWithBomb(pBlob, iBomb, pBomb);
}

void hotelOfXXs_raid(void) {
  XXBomb bomb; // Bomb copied out to here
  Tocks now = tocksNow();
  while (true) { // Returns when nothing to kill for now
    bomb.who = badXXBlobIx; // Prevent false alarms
    Chomped ch = meapOfXXBombs_chomp(now, &bomb, 0); // Locks, so each bomb appears here at most once.
    if (ch == Killed ) {    // ... Also calls onXXBombMeap_willErase and erases the bomb if it says so.
      XXBlob * pBlob = pileOfXXBlobs_get(bomb.who);

      Nick got = atomic_load(&pBlob->rent.nick); 
      Nick flags = got & NICK_FLAG_MASK;
      if (!(flags & NICK_FLAG_BOMBED)) DIE("should be bombed already"); // Either chomp set it or it was set already.
      if (!(flags & NICK_FLAG_BUSY)) { // Normal, idle rent expiry
        printf("Killing idle XX %d\n", bomb.who.i);
        hotelOfXXs_collectRent(&pBlob->rent);
        if (pBlob->rent.cash<0) {
          onXXHotel_rentDefaulted(-pBlob->rent.cash); // Should be at the real free
          pBlob->rent.cash = 0; // Maybe redundant
        }
        onXXHotel_goDie((XXIx){bomb.who.i}, &pBlob->body);
        atomic_store(&pBlob->rent.nick, BAD_INDEX); 
        pileOfXXBlobs_free(bomb.who);
        printf("hotelOfXXs_raid: freed blob\n");
      } else { // Expired when busy. Do nothing - the core will handle it
      }
    }
    if (ch == Extinct) {
      onXXHotel_extinct(); 
      meapOfXXBombs_check();
      return;
    }
    return; // Must be Idle
  }
}

// While it was grabbed, the grabbing party was responsible for
// charging memory rent. Pass the right amount of cash with rent
// paid up to the current tock.
void hotelOfXXs_drop(XXIx i, Cash cash) {
  printf("Dropping XX %d\n", i.i);
  XXBlobIx iBlob = (XXBlobIx){i.i};
  XXBlob * pBlob = pileOfXXBlobs_get(iBlob);
  pBlob->rent.cash = cash;
  pBlob->rent.lastPaidRent = tocksNow();
  printf("BOMBING: hotelOfXXs_drop %d\n", i.i);
  Nick was = atomic_fetch_or(&pBlob->rent.nick, NICK_FLAG_BOMBED); // I might be lying about intending to free the bomb, but it stops raid from doing so.
  printf("Nick was: %-8x\n", was);
  if (was & NICK_NAME_GOD) {
    printf("It's a god\n");
  }                              
  else if (pBlob->rent.cash>0) {         
    if (was & NICK_FLAG_BOMBED) {
      printf("Rebombing\n");
      rebomb(&pBlob->rent, iBlob);
    }
    else {
      printf("Updating\n");
      updateDeath(pBlob);
    }
    printf("UNBOMBING: hotelOfXXs_drop %d\n", i.i);
    atomic_store(&pBlob->rent.nick, was & NICK_NAME_READ_MASK); // Clear both flags
  } else { // Bankrupted by its own code
    printf("Skint ... ");
    if (!(was & NICK_FLAG_BOMBED)) {
      printf("not bombed\n");
      meapOfXXBombs_erase(pBlob->rent.bomb);
    } else {
      printf("already bombed\n");
    }
    onXXHotel_goDie(i, &pBlob->body);
    atomic_store(&pBlob->rent.nick, BAD_INDEX); 
    pileOfXXBlobs_free(iBlob);
    printf("hotelOfXXs_drop: freed blob\n");
    // TODO: Book loss
  }
  hotelOfXXs_raid();
}


