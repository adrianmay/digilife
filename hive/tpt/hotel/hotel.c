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
  //meapOfXXBombs_check();
}

void onXXBombMeap_new(XXBomb * pBomb, XXBombIx i, Ix hint) {
  pBomb->who = (XXBlobIx){hint};
  onXXBombMeap_move(pBomb, i);
}

static uint64_t nb2i(Nick n, XXBombIx b) { XXInb inb; inb.nb.n = n; inb.nb.b = b; return inb.i; }
static XXNb i2nb(uint64_t i ) { XXInb inb; inb.i = i; return inb.nb; }

static Woth eraseBombForTact(XXTact t, V erase) {
  XXBlob * pBlob = pileOfXXBlobs_get((XXBlobIx){t.i.i});
  uint64_t set = nb2i(0, (XXBombIx){BAD_INDEX});
  uint64_t wasi = atomic_fetch_or(&pBlob->rent.inb.i, set);
  XXNb wasnb = i2nb(wasi);
  if (wasnb.n != t.n) {
    atomic_store(&pBlob->rent.inb.i, wasi);
    return Dead;
  }
  if (wasnb.b.i == BAD_INDEX) {
    return Busy;
  }
  if (wasnb.b.i != GOD_BOMB) 
    erase();
  return (Ok); // Got an exclusive lock
}

static bool eraseBombForBlob(XXBlob * pBlob, V erase) {
  // We have to trust that bombs point to valid blobs cos tacts are too big to be in bombs.
  uint64_t set = nb2i(0, (XXBombIx){BAD_INDEX});
  uint64_t wasi = atomic_fetch_or(&pBlob->rent.inb.i, set);
  XXNb wasnb = i2nb(wasi);
  if (wasnb.b.i == BAD_INDEX)
    return false; // Already locked
  if (wasnb.b.i != GOD_BOMB)  {
    erase();
  }
  return true;
}

void onXXBombMeap_timeout(XXBomb * pBomb, XXBombIx i, V erase, V unlock) {
  XXBlobIx iBlob = pBomb->who;
  XXBlob * pBlob = pileOfXXBlobs_get(iBlob);
  if (eraseBombForBlob(pBlob, erase)) {
    unlock();
    onXXHotel_funeral((XXIx){iBlob.i}, &pBlob->body);
    pileOfXXBlobs_free(iBlob);
  } else unlock(); // Otherwise it's up to the thing that locked the blob by deleting the bomb
}

static bool isGod(XXRent * pRent) { return pRent->inb.nb.n & NICK_NAME_GOD ; }

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

bool hotelOfXXs_grabIx(XXIx i, XX ** ppXX, Cash * pCash) {
  if (ppXX) *ppXX = 0;
  XXBlob * pBlob = pileOfXXBlobs_get((XXBlobIx){i.i});
  XXBombIx iBomb = pBlob->rent.inb.nb.b;
  void erase(void) { meapOfXXBombs_erase(iBomb); }

  if (!eraseBombForBlob(pBlob, erase)) {
    return false;
  }
  if (ppXX) *ppXX = &pBlob->body;
  hotelOfXXs_collectRent(&pBlob->rent);
  *pCash = pBlob->rent.cash;
  return true;
}

Woth hotelOfXXs_grab(XXTact t, XX ** ppXX, Cash * pCash) {
  if (ppXX) *ppXX = 0;
  XXBlob * pBlob = pileOfXXBlobs_get((XXBlobIx){t.i.i});
  XXBombIx iBomb = pBlob->rent.inb.nb.b;
  void erase(void) { meapOfXXBombs_erase(iBomb); }
  Woth w = eraseBombForTact(t, erase);
  if (w != Ok) return w;
  if (ppXX) *ppXX = &pBlob->body;
  hotelOfXXs_collectRent(&pBlob->rent);
  *pCash = pBlob->rent.cash;
  return Ok;
}
 
int hotelOfXXs_showsTact(char * cursor, XXTact t) {
  return sprintf(cursor, "%-8x=%d", t.n, t.i.i);
}

void showXXBlob(XXBlobIx i, XXBlob * p) {
  char busy   = (p->rent.inb.nb.b.i == BAD_INDEX) ? 'U' : 'u';
  char god    = (isGod(&p->rent)                ) ? 'G' : 'g';
  Ix base     = (p->rent.inb.nb.n & NICK_NAME_RAND_MASK);
  printf("ix=%-4d nick=%c%c`%07x|lastPaidRent=%-5d cash=%-8ld bomb=%-2d ", i.i, busy, god, base, p->rent.lastPaidRent, p->rent.cash, p->rent.inb.nb.b.i);
  showXX((XXIx){i.i}, &p->body);
}

void showXXBomb(XXBombIx i, XXBomb * p) {
  printf("tocks=%d,who=%d", p->tocks, p->who.i);
}

void showXXPair(XXIx i, XX * p) {
  XXBlob * pBlob = pileOfXXBlobs_get((XXBlobIx){i.i});
  XXBomb * pBomb = meapOfXXBombs_get(pBlob->rent.inb.nb.b);
  showXXBlob((XXBlobIx){i.i} , pBlob);
  printf("¬");
  showXXBomb(pBlob->rent.inb.nb.b, pBomb);
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
  meapOfXXBombs_insert(expiry, i.i); // Do we need the return value?
}
 
XXTact hotelOfXXs_admit(Cash cash, bool isGod, V_XXP stuff, XX ** pp, bool * pRecycled) {
  if (cash == 0 && !isGod) 
    return (XXTact){(XXIx){BAD_INDEX}};
  XXBlob * pBlob;
  XXBlobIx iBlob = pileOfXXBlobs_alloc(&pBlob, pRecycled);
  pBlob->rent.inb.nb.b = badXXBombIx;
  pBlob->rent.cash = cash;
  pBlob->rent.lastPaidRent = tocksNow();
  Nick n = randInt32Masked(NICK_NAME_RAND_MASK) | ( isGod ? NICK_NAME_GOD : 0 );
  pBlob->rent.inb.nb.n = n;
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

XX * hotelOfXXs_get(XXIx i) { 
  XXBlob * pBlob = pileOfXXBlobs_get((XXBlobIx){i.i});
  return &pBlob->body;
}

void hotelOfXXs_raid(void) {
  XXBomb bomb; // Bomb copied out to here
  Tocks now = tocksNow();
  while (true) { // Returns when nothing to kill for now
    bomb.who = badXXBlobIx; // Prevent false alarms
    Chomped ch = meapOfXXBombs_chomp(now, &bomb); // Locks, so each bomb appears here at most once.
    //printf("hotelOfXXs_raid: chomp res %d\n", ch);
    if (ch == Killed); // Already handled vi onXXMeap_timeout
    else if (ch == Extinct) {
      onXXHotel_extinct(); 
      //meapOfXXBombs_check();
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
  if (pBlob->rent.inb.nb.n & NICK_NAME_GOD) {
    pBlob->rent.inb.nb.b = (XXBombIx){GOD_BOMB};
  } else {
    if (cash > 0) {
      pBlob->rent.lastPaidRent = tocksNow();
      rebomb(&pBlob->rent, iBlob);
    } else {
      // TODO: Book negative cash loss
      onXXHotel_funeral((XXIx){iBlob.i}, &pBlob->body);
      pileOfXXBlobs_free(iBlob);
    }
  }
  hotelOfXXs_raid();
}
