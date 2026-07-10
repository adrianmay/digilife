
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
// That's true because onXXMeapNew only called from meap's insert
//   which for the hotel is only called from hotel's admit
// We know it exists, and it doesn't have or need money
void onXXBombMeapNew(XXBomb * pBomb, Ix hint) {
  pBomb->who = (XXBlobIx){hint};
//  XX * p = pileOfXXs_get(pBomb->who);
//  p->rent.bomb = iBomb;
//  updateDeathWithXXAndBomb_(p, pBomb);
//  meapOfXXBombs_check();
}

// Similarly thread safe already, I think?
void onXXBombMeapMove(XXBomb * pBomb, XXBombIx to) {
  XXBlob * pBlob = pileOfXXBlobs_get(pBomb->who);
  pBlob->rent.bomb = to;
  meapOfXXBombs_check();
}

bool onXXBombMeapWillErase(XXBombIx i, XXBomb * pBomb) {
  XXBlobIx who = pBomb->who;
  XXBlob * pBlob = pileOfXXBlobs_get(who);
  Nick was = atomic_fetch_or(&pBlob->rent.nick, NICK_FLAG_BOMBED);
  //if (!(was & NICK_FLAG_BUSY)) // Comes later
  //  pileOfXXs_free(who);
  return (!(was & NICK_FLAG_BOMBED)); // Must remove bomb for meap to continue unless drop already removed it
}


static int numGods=0;

void showXXBomb(XXBlobIx i, XXBomb * p) {
  printf("tocks=%d,who=%d\n", p->tocks, p->who.i);
}

void showXXBlob(XXBlobIx i, XXBlob * p) {
  printf("ix=%-4d nick=%-8x|lastPaidRent=%-5d cash=%-5ld bomb=%-2d ", i.i, p->rent.nick, p->rent.lastPaidRent, p->rent.cash, p->rent.bomb.i);
  showXX(&p->body);
}

void hotelOfXXs_show(void) {
  meapOfXXBombs_show();
  pileOfXXBlobs_show(false);
}

bool hotelOfXXs_open() {
  meapOfXXBombs_open();
  bool virgin = pileOfXXBlobs_open();
  return virgin;
}

Ix hotelOfXXs_count(void) {
  return pileOfXXBlobs_count();
}

void hotelOfXXs_close(Fate fate) {
  pileOfXXBlobs_close(fate);
  meapOfXXBombs_close(fate);
}

const size_t billableXXSize = sizeof(XX)+sizeof(XXBomb);
Cash rentForXXPerTock() { return tockPrice() * billableXXSize; }

static void rebomb(XXRent * pRent, XXBlobIx i) {
  Tocks expiry = pRent->lastPaidRent + pRent->cash / rentForXXPerTock();
  meapOfXXBombs_insert(expiry, i.i, &pRent->bomb); // Do we need the return value?
}
 
XXTact hotelOfXXs_admit(Cash cash, bool isGod, V_XXP stuff, XX ** pp, bool * pRecycled) {
  XXBlob * pBlob;
  XXBlobIx iBlob = pileOfXXBlobs_alloc(&pBlob, pRecycled);
  Nick n = randInt32Masked(NICK_NAME_RAND_MASK) | ( isGod ? NICK_NAME_GOD : 0 );
  pBlob->rent.nick = n;
  XXTact t = (XXTact){(XXIx){iBlob.i}, n};
  if (isGod) { //God
    numGods++;
    pBlob->rent.cash = 0;
    pBlob->rent.lastPaidRent = tocksNow();
    if (stuff) stuff(&pBlob->body);
    pBlob->rent.bomb = badXXBombIx;
    pileOfXXBlobs_modUsr(1); // This is the god count
  } else {
    if (cash == 0) 
      return (XXTact){(XXIx){BAD_INDEX}};
    pBlob->rent.cash = cash;
    pBlob->rent.lastPaidRent = tocksNow();
    if (stuff) stuff(&pBlob->body);
    //printf("Admit: expiry=%d lastPaid=%d cash=%ld\n", expiry, p->rent.lastPaidRent, cash);
    rebomb(&pBlob->rent, iBlob);
  }
  if (pp) *pp = &pBlob->body;
  //printf("admit:nick=%d\n", p->rent.nick);
  return t;
}

static bool isGod(XXRent * pRent) { return pRent->nick & NICK_NAME_GOD ; }

void hotelOfThings_collectRent(XXRent * pRent) {
  Cash collected, defaulted;
  Tocks now = tocksNow();
  Tocks timeUnpaid = now - pRent->lastPaidRent;
  pRent->lastPaidRent = now;
  Cash bill = rentForXXPerTock() * timeUnpaid;
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
  if (collected) onXXRentCollected(collected);
  if (defaulted) onXXRentDefaulted(defaulted);
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
XX * hotelOfThings_grab(XXTact t, Cash * pCash) {
  XXBlob * pBlob = pileOfXXBlobs_get((XXBlobIx){t.i.i});
  Nick want, set; 
  want = t.n; set = t.n | NICK_FLAG_BUSY; 
  if (atomic_compare_exchange_strong(&pBlob->rent.nick, &want, set)) {
    hotelOfThings_collectRent(&pBlob->rent);
    *pCash = pBlob->rent.cash;
    return &pBlob->body;
  }
  else
    return 0;
}
 
// As above if you don't know the nick cos you're the bomb.
// I don't want to bloat the bombs. 
// Since only raid removes XXs, I think I can assume the nick is correct.
XX * hotelOfThings_grabIx(XXIx i, Cash * pCash) {
  XXBlob * pBlob = pileOfXXBlobs_get((XXBlobIx){i.i});
  XXTact t = (XXTact){i, pBlob->rent.nick};
  return hotelOfThings_grab(t, pCash);
}

static bool updateDeathWithBomb(XXBlob * p, XXBombIx iBomb, XXBomb * pBomb) {
  Cash cash = p->rent.cash;
  Tocks ttl = cash / rentForXXPerTock();
  Tocks death = tocksNow() + ttl;
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

void hotelOfThings_drop(XXIx i, Cash cash) {
  XXBlobIx iBlob = (XXBlobIx){i.i};
  XXBlob * pBlob = pileOfXXBlobs_get(iBlob);
  Nick was = atomic_fetch_or(&pBlob->rent.nick, NICK_FLAG_BOMBED); // I might be lying about intending to free the bomb,
  if (was & NICK_NAME_GOD) return;                               //  but it stops raid from doing so.
  if (pBlob->rent.cash>0) {         
    if (was & NICK_FLAG_BOMBED)
      rebomb(&pBlob->rent, iBlob);
    else 
      updateDeath(pBlob);
    atomic_store(&pBlob->rent.nick, was & NICK_NAME_READ_MASK); // Clear both flags
  } else { // Bankrupted by its own code
    if (!(was & NICK_FLAG_BOMBED)) {
      meapOfXXBombs_erase(pBlob->rent.bomb);
    }
    onXXHotelGoDie(i, &pBlob->body);
    atomic_store(&pBlob->rent.nick, BAD_INDEX); 
    pileOfXXBlobs_free(iBlob);
    // TODO: Book loss
  }
}

void hotelOfXXs_raid(void) {
  XXBomb bomb; // Bomb copied out to here
  Tocks now = tocksNow();
  while (true) { // Returns when nothing to kill for now
    bomb.who = badXXBlobIx; // Prevent false alarms
    Chomped ch = meapOfXXBombs_chomp(now, &bomb, 0); // Locks, so each bomb appears here at most once.
    if (ch == Killed ) {    // ... Also calls onXXBombMeapWillErase and erases the bomb if it says so.
      XXBlob * pBlob = pileOfXXBlobs_get(bomb.who);

      Nick got = atomic_load(&pBlob->rent.nick); 
      Nick flags = got & NICK_FLAG_MASK;
      if (!(flags & NICK_FLAG_BOMBED)) abort(); // Either chomp set it or it was set already.
      if (!(flags & NICK_FLAG_BUSY)) { // Normal, idle rent expiry
        hotelOfXXs_collectRent(&pBlob->rent);
        if (pBlob->rent.cash<0) {
          onXXRentDefaulted(-pBlob->rent.cash); // Should be at the real free
          pBlob->rent.cash = 0; // Maybe redundant
          onXXHotelGoDie((XXIx){bomb.who.i}, &pBlob->body);
          atomic_store(&pBlob->rent.nick, BAD_INDEX); 
          pileOfXXBlobs_free(bomb.who);
        }
      } else { // Expired when busy. Do nothing - the core will handle it
      }
    }
    if (ch == Extinct) {
      onXXsExtinct(); 
      meapOfXXBombs_check();
      return;
    }
    return; // Must be Idle
  }
}


