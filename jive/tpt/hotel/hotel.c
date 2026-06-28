#include <pthread.h>
#include <string.h>
#include "types.h"
#include "misc/api.h"
#include "globals/api.h"
#include "XXBomb_pile/record.h"
#include "XX_pile/record.h"
#include "XX_pile/api.h"
#include "XXBomb_meap/api.h"
#include "api.h"

static void show(void) {
  meapOfXXBombs.show();
  pileOfXXs.show(false);
}

static bool open() {
  meapOfXXBombs.open();
  bool virgin = pileOfXXs.open();
  return virgin;
}

static void close(Fate fate) {
  pileOfXXs.close(fate);
  meapOfXXBombs.close(fate);
}

static Ix count(void) {
  return pileOfXXs.count();
}

static XX * get(XXIx i) {
  return pileOfXXs.get(i);
}

static Woth with_(XXTact t, V_XXP act) {
  XX * pXX = hotelOfXXs.get(t.i);
  Nick want, set; 
  want = (t.n); set = t.n | NICK_FLAG_BUSY; 
  if (atomic_compare_exchange_strong(&pXX->rent.nick, &want, set)) {
    act(pXX);
    want = (t.n | NICK_FLAG_BUSY); set = t.n; 
    if (atomic_compare_exchange_strong(&pXX->rent.nick, &want, set))
      return Ok;
    want = (t.n | NICK_FLAG_BUSY | NICK_FLAG_BOMBED); set = t.n | NICK_FLAG_BUSY | NICK_FLAG_BOMBED; 
    if (atomic_compare_exchange_strong(&pXX->rent.nick, &want, set)) {
      onXXHotelGoDie(t.i, pXX);
      pileOfXXs.free(t.i); // Free it
      return Ok;
    }
  } else {
    want = t.n | NICK_FLAG_BUSY; set = t.n | NICK_FLAG_BUSY; 
    if (atomic_compare_exchange_strong(&pXX->rent.nick, &want, set)) {
      return Busy;
    }
  }
  return Dead;
}

static Woth with(XXTact t, V_XXP act) {
  XX * pXX = hotelOfXXs.get(t.i);
  Woth w = with_(t, act);
  Nick n = atomic_load(&pXX->rent.nick);
  if (n==0) {
    printf("Gotcha 3\n");
    abort();
  }
  return w;
}

static Woth withIx(XXIx i, V_XXP act) {
  XX * pXX = hotelOfXXs.get(i);
  XXTact t = (XXTact){i, pXX->rent.nick};
  return with(t, act);
}
 
static int numGods=0;

static void checkHotel(int expectExcess) {
  int diff = pileOfXXs.count() - (meapOfXXBombs.size() + numGods + expectExcess);
  if (diff) {
    //printf("checkHotel failed; %d less bombs.\n", diff);
    //abort();
  }
}

void showXXBomb(XXBomb * p) {
  printf("tocks=%d,who=%d\n", p->tocks, p->who.i);
}

//Below, the trailing underscore means no lock taken

// Updates bomp expiry and reorders meap. 
// Assumes collectRent was just called.
static bool updateDeathWithXXAndBomb_(XX* p, XXBombIx iBomb, XXBomb * pBomb) {
  if (pBomb->who.i==194 && iter>=1370 ) { 
    printf("##################### %d\n", iter); 
    showXXBomb(pBomb);
    show();
  }
  //if (pBomb->who.i==194) printf(" ###################### iter=%d\n", iter);
  Cash cash = p->rent.cash;
  Tocks ttl = cash / ( tockPrice() * billableXXSize);
  Tocks death = tocksNow() + ttl;
  meapOfXXBombs.check();
  // This changes bomb time and reorders meap:
  bool res = false;
  if (iter>=1368 ) {
    printf(">>>>>>>>>>>>>>>>>>>>> \n"); 
    show();
    printf("<<<<<<<<<<<<<<<<<<<<< \n"); 
    printf("Bomb ixs: %d %d\n", p->rent.bomb.i, iBomb.i);
  }
  if (p->rent.bomb.i == iBomb.i)
    res  = meapOfXXBombs.editTocks(p->rent.bomb, death);
  
  if (pBomb->who.i==194 && iter>=1370 ) { 
    showXXBomb(pBomb);
    show();
    printf("##################### %d\n", iter); 
    abort();
  }
  meapOfXXBombs.check();
  return res;
  // The meap is now properly reordered but nobody called kill
}

// Wrappers:
static bool updateDeathWithXX_(XX * pXX) {
  XXBombIx iBomb = pXX->rent.bomb;
  XXBomb * pBomb = meapOfXXBombs.get(iBomb);
  return updateDeathWithXXAndBomb_(pXX, iBomb, pBomb);
}

// Debug:
//static XXIx bombee;
//static void bombeeSafe(Ix i, void * p) { 
//  XXBomb * pB = (XXBomb *) p;
//  if (pB->who.i == bombee.i) {
//    printf("After chomp: Another bomb for XX %d\n", bombee.i);
//    abort();
//  }
//}
//static void bombeeSafe2(Ix i, void * p) { 
//  XXBomb * pB = (XXBomb *) p;
//  if (pB->who.i == bombee.i) {
//    printf("In admit: Another bomb for XX %d\n", bombee.i);
//    abort();
//  }
//}

static bool isGod(XX * pXX) { return pXX->rent.nick & NICK_NAME_GOD ; }

//Deduct cash and set last paid to now
//Catches defaults now
static void collectRent(XX * pXX, bool updateBomb) {
  Cash collected, defaulted;
  XXRent * pRent = &pXX->rent;
  Tocks now = tocksNow();
  Tocks timeUnpaid = now - pRent->lastPaidRent;
  pRent->lastPaidRent = now;
  Cash bill = tockPrice() * billableXXSize * timeUnpaid;
  if (isGod(pXX) || pRent->cash >= bill) {
    pRent->cash -= bill;
    collected = bill;
    defaulted = 0;
  } else {
    Cash c = pRent->cash;
    pRent->cash = 0;
    collected = c;
    defaulted = bill-c;
    if (updateBomb) updateDeathWithXX_(pXX);
  }
  if (collected) onXXRentCollected(collected);
  if (defaulted) onXXRentDefaulted(defaulted);
}

//static int chance=0;
static void raid(void) {
  //printf("Raid starts\n");
  checkHotel(0);
  XXBomb bomb; // Bomb copied out to here
  Tocks now = tocksNow();
  //printf("Tocks=%d\n", now);
  //show();
  while (true) { // Returns when nothing to kill for now
    bomb.who = badXXIx; // Prevent false alarms
    //meapOfXXBombs.show();
    Chomped ch = meapOfXXBombs.chomp(now, &bomb, 0);
    if (ch == Killed ) {
      //bombee = bomb.who;
      //meapOfXXBombs.forAll(bombeeSafe);
      meapOfXXBombs.check();
      if (bomb.who.i == 194 && bomb.tocks==1138) {//printf("Chomped number 194, tocks=%d\n", bomb.tocks);
        show();
        printf("iter=%d\n", iter);
        abort();
      }
      XX * pXX = hotelOfXXs.get(bomb.who);
      //printf("XX Chomped with expiry=%d, who=%d, cash before collecting rent=%ld\n", bomb.tocks, bomb.who.i, pXX->rent.cash);
      collectRent(pXX, false);
      if (pXX->rent.cash<0) {
        onXXRentDefaulted(-pXX->rent.cash); // Should be at the real free
        pXX->rent.cash = 0;
      }
      Nick want = atomic_fetch_or(&pXX->rent.nick, NICK_FLAG_BOMBED); 
      if (want & NICK_FLAG_BOMBED) {
        printf("XX %d already doomed\n", bomb.who.i);
        show();
        abort();
      }
      if (want & NICK_FLAG_BUSY) continue;
      Nick test = atomic_exchange(&pXX->rent.nick, BAD_INDEX); 
      if (test == 0) {
        printf("Gotcha\n");
        abort();
      }
      //printf("Free in raid\n");
      onXXHotelGoDie(bomb.who, pXX);
      pileOfXXs.free(bomb.who);
      continue;
    }
    if (ch == Extinct) {
      checkHotel(0);
      onXXsExtinct(); 
      meapOfXXBombs.check();
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

static void richer(XX * pXX, Cash amt) {
  if (amt<0) abort();
  if (amt==0) return;
  changeCash_(pXX, amt);
}

static Cash poorer(XX * pXX, Cash amt, Terms t) {
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

static Cash rob(XX * pXX) {
  return poorer(pXX, 0, Rob);
}

void showXX(XXTact t, XX * p) {
  printf("ix=%-4d nick=%-8x|lastPaidRent=%-5d cash=%-5ld bomb=%-2d ", t.i.i, p->rent.nick, p->rent.lastPaidRent, p->rent.cash, p->rent.bomb.i);
  showXXBody(&p->body);
}

static XXTact admit(Cash cash, bool isGod, V_XXBodyP stuff, XX ** pp, bool * pRecycled) {
  checkHotel(0);
  XX * p;
  XXIx i = pileOfXXs.alloc(&p, pRecycled);
  Nick n = randInt32Masked(NICK_NAME_RAND_MASK) | ( isGod ? NICK_NAME_GOD : 0 );
  XXTact t = (XXTact){i, n};
  p->rent.nick = n;
  if (isGod) { //God
    numGods++;
    p->rent.cash = 0;
    p->rent.lastPaidRent = tocksNow();
    if (stuff) stuff(&p->body);
    p->rent.bomb = badXXBombIx;
    pileOfXXs.modUsr(1);
  } else {
    if (cash == 0) return (XXTact){(XXIx){BAD_INDEX}};
    p->rent.cash = cash;
    p->rent.lastPaidRent = tocksNow();
    if (stuff) stuff(&p->body);
    Tocks expiry = p->rent.lastPaidRent + cash / (billableXXSize * tockPrice());
    //printf("Admit: expiry=%d lastPaid=%d cash=%ld\n", expiry, p->rent.lastPaidRent, cash);
    meapOfXXBombs.insert(expiry, i.i, &p->rent.bomb); // Do we need the return value?
  }
  if (pp) *pp = p;
  //printf("admit:nick=%d\n", p->rent.nick);
  return t;
}


// Assumes some mutex is held, despite the name
// That's true because onXXMeapNew only called from meap's insert
//   which for the hotel is only called from hotel's admit
// We know it exists, and it doesn't have or need money
void onXXBombMeapNew(XXBomb * pBomb, Ix hint) {
  pBomb->who = (XXIx){hint};
//  XX * p = pileOfXXs.get(pBomb->who);
//  p->rent.bomb = iBomb;
//  updateDeathWithXXAndBomb_(p, pBomb);
//  meapOfXXBombs.check();
}

// Similarly thread safe already, I think?
void onXXBombMeapMove(XXBomb * pBomb, XXBombIx to) {
  XX * p = pileOfXXs.get(pBomb->who);
  p->rent.bomb = to;
  meapOfXXBombs.check();
}

Nick onXXBombMeapWillErase(XXBombIx i, XXBomb * pBomb) {
  XX * pXX = pileOfXXs.get(pBomb->who);
  return atomic_fetch_or(&pXX->rent.nick, NICK_FLAG_BOMBED);
}

static void forAll(bool u, V_XXI_XXP act) { 
  pileOfXXs.forAll(false, act); 
}

XXHotel hotelOfXXs = { open, admit, get, with, withIx,
  richer, poorer, rob, collectRent, forAll, raid, 
  count, checkHotel, close, show, showXX };


const size_t billableXXSize = sizeof(XX)+sizeof(XXBomb);
