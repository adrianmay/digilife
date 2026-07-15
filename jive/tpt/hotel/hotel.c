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

// Similarly thread safe already, I think?
void onXXBombMeap_move(XXBomb * pBomb, XXBombIx to) {
  XXBlob * pBlob = pileOfXXBlobs_get(pBomb->who);
  pBlob->rent.inb.nb.b = to;
  meapOfXXBombs_check();
}

void onXXBombMeap_new(XXBomb * pBomb, XXBombIx i, Ix hint) {
  pBomb->who = (XXBlobIx){hint};
  onXXBombMeap_move(pBomb, i);
}

uint64_t n_b2i(Nick n, XXBombIx i) { XXInb inb; inb.nb.n = n; inb.nb.i = i; return inb.i; }
uint64_t nb2i(XXNb nb) { XXInb inb; inb.nb = nb; return inb.i; }
XXNb i2nb(uint64_t i ) { XXInb inb; inb.i = i; return inb.nb; }

static bool eraseBombForBlob(XXBlob * pBlob, V erase) {
  // Gods have -2 for rent.bomb
}

static Woth eraseBombForTact(XXTact * t, V erase) {
  uint64_t set = n_b2i(0, 0xFFFFFFFF);
  uint64_t wasi = atomic_fetch_or(&pBlob->rent.inb.i, set);
  XXNb wasnb = i2nb(wasi);
  if (wasnb.n != t.n) {
    atomic_store(&pBlob->rent.inb.i, wasi);
    return Dead;
  }
  if (wasnb.b == 0xFFFFFFFF) {
    atomic_store(&pBlob->rent.inb.i, wasi);
    return Busy;
  }
  bool match = wasnb.n == t.n && wasnb.b != 0xFFFFFFFF;
  if (wasnb.b =!= set.i && was.i != GOD_BOMB) erase();
  return (was != set); // Got an exclusive lock
}

void onXXBombMeap_timeout(XXBomb * pBomb, XXBombIx i, V erase) {
  eraseBombForBlob(pileOfXXBlobs_get(pBomb->who), erase); //Must exist if the bomb does
}

bool hotelOfXXs_grabIx(XXIx i, XX * p, Cash * pCash) {
  XXBlob * pBlob = pileOfXXBlobs_get((XXBlobIx){i.i});
  void erase(void) { meapOfXXBombs_erase(pBlob->rent.bomb); }
  return eraseBombForBlob(pBlob, erase);
}

Woth hotelOfXXs_grab(XXTact t, XX * p, Cash * pCash) {
  XXBlob * pBlob = pileOfXXBlobs_get((XXBlobIx){t.i.i});
  if (pBlob->rent.name != t.n) return Dead;
  Nick want, set; 
  want = t.n; set = t.n | NICK_FLAG_BUSY; 
  if (atomic_compare_exchange_strong(&pBlob->rent.nick, &want, set)) {
    hotelOfXXs_collectRent(&pBlob->rent);
    *pCash = pBlob->rent.cash;
    return &pBlob->body;
  }
  else {
    printf("Failed to grab %d\n", t.i.i);
    //if (want == 0xFFFFFFFF) return 0;
    //DIE("hotelOfXXs_grab: failed to grab %d\n", t.i.i);
    return 0;
  }
}
 
int hotelOfXXs_showsTact(char * cursor, XXTact t) {
  return sprintf(cursor, "%-8x=%d", t.n, t.i.i);
}

void showXXBlob(XXBlobIx i, XXBlob * p) {
  char busy   = (p->rent.nick & NICK_FLAG_BUSY  ) ? 'U' : 'u';
  char god    = (p->rent.nick & NICK_NAME_GOD   ) ? 'G' : 'g';
  Ix base     = (p->rent.nick & NICK_NAME_RAND_MASK);
  printf("ix=%-4d nick=%c%c`%07x|lastPaidRent=%-5d cash=%-8ld bomb=%-2d ", i.i, busy, god, base, p->rent.lastPaidRent, p->rent.cash, p->rent.bomb.i);
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
  //printf("rebomb: expiry: %d cash: %'ld, rent: %f\n", expiry, pRent->cash, hotelOfXXs_rent());
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

}
void hotelOfXXs_raid(void) {
  XXBomb bomb; // Bomb copied out to here
  Tocks now = tocksNow();
  while (true) { // Returns when nothing to kill for now
    bomb.who = badXXBlobIx; // Prevent false alarms
    Chomped ch = meapOfXXBombs_chomp(now, &bomb, 0); // Locks, so each bomb appears here at most once.
    //printf("hotelOfXXs_raid: chomp res %d\n", ch);
    if (ch == Killed ) { 
      XXBlob * pBlob = pileOfXXBlobs_get(bomb.who);
      Nick got = atomic_fetch_or(&pBlob->rent.nick, NICK_FLAG_BUSY); 
      if (!(got & NICK_FLAG_BUSY)) { // Normal, idle rent expiry
        hotelOfXXs_collectRent(&pBlob->rent);
        if (pBlob->rent.cash<0) {
          onXXHotel_rentDefaulted(-pBlob->rent.cash); // Should be at the real free
          pBlob->rent.cash = 0; // Maybe redundant
        }
        onXXHotel_goDie((XXIx){bomb.who.i}, &pBlob->body);
        atomic_store(&pBlob->rent.nick, BAD_INDEX); 
        pileOfXXBlobs_free(bomb.who);
      } else { // Expired when busy. Do nothing - the core will handle it
      }
    }
    else if (ch == Extinct) {
      onXXHotel_extinct(); 
      meapOfXXBombs_check();
      return;
    }
    else return; // Must be Idle
  }
}

// While it was grabbed, the grabbing party was responsible for
// charging memory rent. Pass the right amount of cash with rent
// paid up to the current tock.
void hotelOfXXs_drop(XXIx i, Cash cash) {
  XXBlobIx iBlob = (XXBlobIx){i.i};
  XXBlob * pBlob = pileOfXXBlobs_get(iBlob);
  pBlob->rent.cash = cash;
  pBlob->rent.lastPaidRent = tocksNow();
  if (was & NICK_NAME_GOD) {
  }                              
  else if (pBlob->rent.cash>0) {         
    rebomb(&pBlob->rent, iBlob);
    atomic_store(&pBlob->rent.nick, was & NICK_NAME_READ_MASK); // Clear both flags
  } else { // Bankrupted by its own code
    onXXHotel_goDie(i, &pBlob->body);
    atomic_store(&pBlob->rent.nick, BAD_INDEX); 
    pileOfXXBlobs_free(iBlob);
    // TODO: Book loss
  }
  hotelOfXXs_raid();
}
