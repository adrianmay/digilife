#include <pthread.h>
#include <stdatomic.h>
#include <string.h>
#include "types.h"
#include "misc/api.h"
#include "globals/api.h"
#include "XXBomb_pile/record.h"
#include "XX_pile/record.h"
#include "XX_pile/api.h"
#include "XXBomb_meap/api.h"
#include "api.h"

void hotelOfThings_show(void) {
  meapOfXXBombs_show();
  pileOfXXs_show(false);
}

bool hotelOfThings_open() {
  meapOfXXBombs_open();
  bool virgin = pileOfXXs_open();
  return virgin;
}

void hotelOfThings_close(Fate fate) {
  pileOfXXs_close(fate);
  meapOfXXBombs_close(fate);
}

Ix hotelOfThings_count(void) {
  return pileOfXXs_count();
}

XX * hotelOfThings_get(XXIx i) {
  return pileOfXXs_get(i);
}

// static Woth with_(XXTact t, V_XXP act) {
//   XX * pXX = hotelOfXXs_get(t.i);
//   Nick want, set; 
//   want = (t.n); set = t.n | NICK_FLAG_BUSY; 
//   if (atomic_compare_exchange_strong(&pXX->rent.nick, &want, set)) {
//     act(pXX);
//     want = (t.n | NICK_FLAG_BUSY); set = t.n; 
//     if (atomic_compare_exchange_strong(&pXX->rent.nick, &want, set))
//       return Ok;
//     want = (t.n | NICK_FLAG_BUSY | NICK_FLAG_BOMBED); set = t.n | NICK_FLAG_BUSY | NICK_FLAG_BOMBED; 
//     if (atomic_compare_exchange_strong(&pXX->rent.nick, &want, set)) {
//       onXXHotelGoDie(t.i, pXX);
//       pileOfXXs_free(t.i); // Free it
//       return Ok;
//     }
//   } else {
//     want = t.n | NICK_FLAG_BUSY; set = t.n | NICK_FLAG_BUSY; 
//     if (atomic_compare_exchange_strong(&pXX->rent.nick, &want, set)) {
//       return Busy;
//     }
//   }
//   return Dead;
// }
// 
// static Woth with(XXTact t, V_XXP act) {
//   XX * pXX = hotelOfXXs_get(t.i);
//   Woth w = with_(t, act);
//   Nick n = atomic_load(&pXX->rent.nick);
//   if (n==0) {
//     printf("Gotcha 3\n");
//     abort();
//   }
//   return w;
// }
// 
// static Woth withIx(XXIx i, V_XXP act) {
//   XX * pXX = hotelOfXXs_get(i);
//   XXTact t = (XXTact){i, pXX->rent.nick};
//   return with(t, act);
// }
 
const size_t billableXXSize = sizeof(XX)+sizeof(XXBomb);
Cash rentForXXPerTock() { return tockPrice() * billableXXSize; }

static int numGods=0;

void hotelOfThings_checkHotel(int expectExcess) {
  int diff = pileOfXXs_count() - (meapOfXXBombs_size() + numGods + expectExcess);
  if (diff) {
    //printf("checkHotel failed; %d less bombs.\n", diff);
    //abort();
  }
}

void showXXBomb(XXIx i, XXBomb * p) {
  printf("tocks=%d,who=%d\n", p->tocks, p->who.i);
}

//Below, the trailing underscore means no lock taken

// Updates bomp expiry and reorders meap. 
// Assumes collectRent was just called.
// Assumes the thing is grabbed.
static bool updateDeathWithXXAndBomb_(XX* p, XXBombIx iBomb, XXBomb * pBomb) {
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

// Wrappers:
static bool updateDeathWithXX_(XX * pXX) {
  XXBombIx iBomb = pXX->rent.bomb;
  XXBomb * pBomb = meapOfXXBombs_get(iBomb); // Assumes grabbed.
  return updateDeathWithXXAndBomb_(pXX, iBomb, pBomb);
}

static void rebomb(XXRent * pRent, XXIx i) {
  Tocks expiry = pRent->lastPaidRent + pRent->cash / rentForXXPerTock();
  meapOfXXBombs_insert(expiry, i.i, &pRent->bomb); // Do we need the return value?
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
XX * hotelOfThings_grab(XXTact t) {
  XX * pXX = pileOfXXs_get(t.i);
  Nick want, set; 
  want = t.n; set = t.n | NICK_FLAG_BUSY; 
  if (atomic_compare_exchange_strong(&pXX->rent.nick, &want, set))
    return pXX;
  else
    return 0;
}
 
// As above if you don't know the nick cos you're the bomb.
// I don't want to bloat the bombs and since only raid removes XXs, I think I can assume the nick is correct.
XX * hotelOfThings_grabIx(XXIx i) {
  XX * pXX = pileOfXXs_get(i);
  XXTact t = (XXTact){i, pXX->rent.nick};
  return hotelOfThings_grab(t);
}

void hotelOfThings_drop(XXIx i) {
  XX * pXX = pileOfXXs_get(i);
  Nick was = atomic_fetch_or(&pXX->rent.nick, NICK_FLAG_BOMBED); // I might be lying about intending to free the bomb,
  if (pXX->rent.cash>0) {                                        //  but it stops raid from doing so.
    printf("Drop: solvent\n");
    if (was & NICK_FLAG_BOMBED)
      rebomb(&pXX->rent, i);
    else 
      updateDeathWithXX_(pXX);
    atomic_store(&pXX->rent.nick, was & NICK_NAME_READ_MASK); // Clear both flags
  } else { // Bankrupted by its own code
    printf("Drop: broke\n");
    if (!(was & NICK_FLAG_BOMBED)) {
        meapOfXXBombs_erase(pXX->rent.bomb);
    }
    onXXHotelGoDie(i, pXX);
    atomic_store(&pXX->rent.nick, BAD_INDEX); 
    pileOfXXs_free(i);
    // TODO: Book loss
  }
}

// Debug:
static XXIx bombee;
static void bombeeSafe(Ix i, void * p) { 
  XXBomb * pB = (XXBomb *) p;
  if (pB->who.i == bombee.i) {
    printf("After chomp: Another bomb for XX %d\n", bombee.i);
    abort();
  }
}
//static void bombeeSafe2(Ix i, void * p) { 
//  XXBomb * pB = (XXBomb *) p;
//  if (pB->who.i == bombee.i) {
//    printf("In admit: Another bomb for XX %d\n", bombee.i);
//    abort();
//  }
//}

static bool isGod(XX * pXX) { return pXX->rent.nick & NICK_NAME_GOD ; }

// Assumes some mutex is held, despite the name
// That's true because onXXMeapNew only called from meap's insert
//   which for the hotel is only called from hotel's admit
// We know it exists, and it doesn't have or need money
void onXXBombMeapNew(XXBomb * pBomb, Ix hint) {
  pBomb->who = (XXIx){hint};
//  XX * p = pileOfXXs_get(pBomb->who);
//  p->rent.bomb = iBomb;
//  updateDeathWithXXAndBomb_(p, pBomb);
//  meapOfXXBombs_check();
}

// Similarly thread safe already, I think?
void onXXBombMeapMove(XXBomb * pBomb, XXBombIx to) {
  XX * p = pileOfXXs_get(pBomb->who);
  p->rent.bomb = to;
  meapOfXXBombs_check();
}

//Deduct cash and set last paid to now
//Catches defaults now
//Assumes grabbed.
void hotelOfThings_collectRent(XX * pXX) {
  Cash collected, defaulted;
  XXRent * pRent = &pXX->rent;
  Tocks now = tocksNow();
  Tocks timeUnpaid = now - pRent->lastPaidRent;
  pRent->lastPaidRent = now;
  Cash bill = rentForXXPerTock() * timeUnpaid;
  if (isGod(pXX) || pRent->cash >= bill) {
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

bool onXXBombMeapWillErase(XXBombIx i, XXBomb * pBomb) {
  XXIx who = pBomb->who;
  XX * pXX = pileOfXXs_get(who);
  Nick was = atomic_fetch_or(&pXX->rent.nick, NICK_FLAG_BOMBED);
  //if (!(was & NICK_FLAG_BUSY)) // Comes later
  //  pileOfXXs_free(who);
  return (!(was & NICK_FLAG_BOMBED)); // Must remove bomb for meap to continue unless drop already removed it
}

void hotelOfThings_raid(void) {
  //printf("Raid starts\n");
  hotelOfThings_checkHotel(0);
  XXBomb bomb; // Bomb copied out to here
  Tocks now = tocksNow();
  //printf("Tocks=%d\n", now);
  //show();
  while (true) { // Returns when nothing to kill for now
    bomb.who = badXXIx; // Prevent false alarms
    //meapOfXXBombs_show();
    //printf("Raid before chomp, hotel is >>>\n");
    //hotelOfXXs_show();
    //printf("<<<\n");
    Chomped ch = meapOfXXBombs_chomp(now, &bomb, 0); // Locks, so each bomb appears here at most once.
    //printf("Raid after chomp, hotel is >>>\n");
    //hotelOfXXs_show();
    //printf("<<<\n");
    if (ch == Killed ) {    // ... Also calls onXXBombMeapWillErase and erases the bomb if it says so.
      bombee = bomb.who;  // ... It says not only if drop already erased it.
      meapOfXXBombs_forAll(bombeeSafe);
      //hotelOfXXs_show();
      meapOfXXBombs_check();
      XX * pXX = hotelOfXXs_get(bomb.who);
      //printf("XX Chomped with expiry=%d, who=%d, cash before collecting rent=%ld\n", bomb.tocks, bomb.who.i, pXX->rent.cash);

      Nick got = atomic_load(&pXX->rent.nick); 
      Nick flags = got & NICK_FLAG_MASK;
      if (!(flags & NICK_FLAG_BOMBED)) abort(); // Either chomp set it or it was set already.
      if (!(flags & NICK_FLAG_BUSY)) { // Normal, idle rent expiry
        hotelOfThings_collectRent(pXX);
        if (pXX->rent.cash<0) {
          onXXRentDefaulted(-pXX->rent.cash); // Should be at the real free
          pXX->rent.cash = 0; // Maybe redundant
          onXXHotelGoDie(bomb.who, pXX);
          atomic_store(&pXX->rent.nick, BAD_INDEX); 
          pileOfXXs_free(bomb.who);
        }
      } else { // Expired when busy. Do nothing - the core will handle it
      }
/*
      collectRent(pXX, false);
      if (pXX->rent.cash<0) {
        onXXRentDefaulted(-pXX->rent.cash); // Should be at the real free
        pXX->rent.cash = 0;
      }
      if (want & NICK_FLAG_BOMBED) {
        printf("XX %d already doomed\n", bomb.who.i);
        show();
        abort();
      }
      if (want & NICK_FLAG_BUSY) continue;
      if (test == 0) {
        printf("Gotcha\n");
        abort();
      }
      //printf("Free in raid\n");
      onXXHotelGoDie(bomb.who, pXX);
      pileOfXXs_free(bomb.who);
      continue;
      */
    }
    if (ch == Extinct) {
      hotelOfThings_checkHotel(0);
      onXXsExtinct(); 
      meapOfXXBombs_check();
      return;
    }
    //printf("Raid over\n");
    return; // Must be Idle
  }
}

static void changeCash_(XX * pXX, Cash amt) {
  pXX->rent.cash += amt;
  if (!isGod(pXX)) 
    updateDeathWithXX_(pXX);
}

void hotelOfThings_richer(XX * pXX, Cash amt) {
  if (amt<0) abort();
  if (amt==0) return;
  changeCash_(pXX, amt);
}

Cash hotelOfThings_poorer(XX * pXX, Cash amt, Terms t) {
  if (amt<0) abort();
  if (amt==0 && t!=Rob) return 0;
  Cash ret;
  if (isGod(pXX)) {
    ret = amt;
  } else {
    Cash got = pXX->rent.cash;
    ret = t==Exact ? (amt<=got ? amt : 0) :
      t==Ono   ? MIN(amt, got) :
      t==Rob   ? got :
      (abort(),0);
  }
  changeCash_(pXX, -ret);
  return ret;
}

Cash hotelOfThings_rob(XX * pXX) {
  return hotelOfThings_poorer(pXX, 0, Rob);
}

void showXX(XXTact t, XX * p) {
  printf("ix=%-4d nick=%-8x|lastPaidRent=%-5d cash=%-5ld bomb=%-2d ", t.i.i, p->rent.nick, p->rent.lastPaidRent, p->rent.cash, p->rent.bomb.i);
  showXXBody(&p->body);
}

XXTact hotelOfThings_admit(Cash cash, bool isGod, V_XXBodyP stuff, XX ** pp, bool * pRecycled) {
  hotelOfThings_checkHotel(0);
  XX * p;
  XXIx i = pileOfXXs_alloc(&p, pRecycled);
  Nick n = randInt32Masked(NICK_NAME_RAND_MASK) | ( isGod ? NICK_NAME_GOD : 0 );
  p->rent.nick = n;
  XXTact t = (XXTact){i, n};
  if (isGod) { //God
    numGods++;
    p->rent.cash = 0;
    p->rent.lastPaidRent = tocksNow();
    if (stuff) stuff(&p->body);
    p->rent.bomb = badXXBombIx;
    pileOfXXs_modUsr(1);
  } else {
    if (cash == 0) 
      return (XXTact){(XXIx){BAD_INDEX}};
    p->rent.cash = cash;
    p->rent.lastPaidRent = tocksNow();
    if (stuff) stuff(&p->body);
    //printf("Admit: expiry=%d lastPaid=%d cash=%ld\n", expiry, p->rent.lastPaidRent, cash);
    rebomb(&p->rent, i);
  }
  if (pp) *pp = p;
  //printf("admit:nick=%d\n", p->rent.nick);
  return t;
}


void hotelOfXXs_forAll(bool u, V_XXI_XXP act) { 
  pileOfXXs_forAll(false, act); 
}

