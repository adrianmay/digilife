#include <pthread.h>
#include <string.h>
#include "types.h"
#include "misc/h.h"
#include "globals/h.h"
#include "XXBomb_meap/2.h"
#include "XX_pile/2.h"
#include "h.h"

static void show(void) {
  meapOfXXBombs.show();
  pileOfXXs.show(false);
}

static bool open() {
  meapOfXXBombs.open();
  bool virgin = pileOfXXs.open();
  return virgin;
}

static void close(FATE fate) {
  pileOfXXs.close(fate);
  meapOfXXBombs.close(fate);
}

static Ix count(void) {
  return pileOfXXs.count();
}

static XX * get(XXIx i) {
  return pileOfXXs.get(i);
}

static Woth with(XXTact t, WithXX act) {
  Nick nick; 
  XX * pXX = hotelOfXXs.get(t.i);
  nick = atomic_fetch_or(&pXX->rent.nick, NICK_BUSY); 
  if (nick==t.n) { // Normal case
    act(pXX);
    nick = atomic_fetch_and(&pXX->rent.nick, ~NICK_BUSY); 
    if (nick == (t.n | NICK_BUSY)) // Normal
      return Ok;
    if (nick | NICK_DOOMED) { // Somebody requested to delete it when we were using it.
      atomic_store(&pXX->rent.nick, BAD_INDEX); 
      if (nick != (t.n | NICK_DOOMED)) abort(); // Shouldn't happen
      pileOfXXs.free(t.i); // Free it
      printf("Late free\n");
      return Ok;
    }
  } 
  if (nick == (t.n | NICK_BUSY)) return Busy;
  return Dead;// Doomed, BAD_INDEX or a new mob. 
}

static int numGods=0;

static void checkHotel(int expectExcess) {
  int diff = pileOfXXs.count() - (meapOfXXBombs.size() + numGods + expectExcess);
  if (diff) {
    //printf("checkHotel failed; %d less bombs.\n", diff);
    //abort();
  }
}

//Below, the trailing underscore means no lock taken

// Updates bomp expiry and reorders meap. 
// Assumes collectRent was just called.
static bool updateDeathWithXXAndBomb_(XX* p, XXBomb * pBomb) {
  Cash cash = p->rent.cash;
  Tocks ttl = cash / ( tockPrice() * billableXXSize);
  Tocks death = tocksNow() + ttl;
  meapOfXXBombs.check();
  // This changes bomb time and reorders meap:
  bool res = meapOfXXBombs.editTocks(p->rent.bomb, death);
  meapOfXXBombs.check();
  return res;
  // The meap is now properly reordered but nobody called kill
}

// Wrappers:
static bool updateDeathWithXX_(XX * pXX) {
  XXBombIx iBomb = pXX->rent.bomb;
  XXBomb * pBomb = pileOfXXBombs.get(iBomb);
  return updateDeathWithXXAndBomb_(pXX, pBomb);
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

void showXXBomb(XXBombIx i, XXBomb * p) {
  printf("tocks=%d,who=%d\n", p->tocks, p->who.i);
}

static bool isGod(XX * pXX) { return pXX->rent.bomb.i == BAD_INDEX; }

static void raid(void) {
  checkHotel(0);
  XXBomb bomb; // Bomb copied out to here
  Tocks now = tocksNow();
  while (true) { // Returns when nothing to kill for now
    bomb.who = badXXIx; // Prevent false alarms
    //meapOfXXBombs.show();
    Chomped ch = meapOfXXBombs.chomp(now, &bomb, 0);
    if (ch == Killed ) {
      //bombee = bomb.who;
      //meapOfXXBombs.forAll(bombeeSafe);
      //printf("XX Chomped with who=%d\n", bomb.who.i);
      meapOfXXBombs.check();
      XX * pXX = hotelOfXXs.get(bomb.who);
      if (pXX->rent.cash<0) {
        onXXRentDefaulted(-pXX->rent.cash); // Should be at the real free
        pXX->rent.cash = 0;
      }
      Nick want;
      want = atomic_fetch_or(&pXX->rent.nick, NICK_DOOMED); 
      if (want & NICK_DOOMED) 
        abort();
      if (want & NICK_BUSY) continue;
      atomic_store(&pXX->rent.nick, BAD_INDEX); 
      printf("Free in raid\n");
      pileOfXXs.free(bomb.who);
      continue;
    }
    if (ch == Extinct) {
      checkHotel(0);
      onXXsExtinct(); 
      meapOfXXBombs.check();
      return;
    }
    return; // Must be Idle
  }
}

//Deduct cash and set last paid to now
//Catches defaults now
static void collectRent_(XX * pXX) {
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
    updateDeathWithXX_(pXX);
  }
  if (collected) onXXRentCollected(collected);
  if (defaulted) onXXRentDefaulted(defaulted);
}

static Woth collectRent(XXTact t) {
  void f(XX * pXX) { collectRent_(pXX);}
  return with(t, f); 
}

static void changeCash_(XX * pXX, Cash amt) {
  pXX->rent.cash += amt;
  if (!isGod(pXX)) 
    updateDeathWithXX_(pXX);
}

static void richer_(XX * pXX, Cash amt) {
  if (amt<0) abort();
  if (amt==0) return;
  changeCash_(pXX, amt);
}

static Woth richer(XXTact t, Cash amt) {
  void f(XX * pXX) { richer_(pXX, amt);}
  return with(t, f); 
}

static Cash poorer_(XX * pXX, Cash amt, Terms t) {
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

static Woth poorer(XXTact t, Cash * amt, Terms terms) {
  void f(XX * pXX) { *amt = poorer_(pXX, *amt, terms);}
  return with(t, f); 
}

void showXX(XXTact t, XX * p) {
  printf("ix=%-4d nick=%-4d|lastPaidRent=%-5d cash=%-5ld bomb=%-2d ", t.i.i, t.n, p->rent.lastPaidRent, p->rent.cash, p->rent.bomb.i);
  showXXBody(t.i, &p->body);
}

static XXTact admit(Cash cash, WithXXBody stuff, XX ** pp, bool * pRecycled) {
  checkHotel(0);
  XX * p;
  XXIx i = pileOfXXs.alloc(&p, pRecycled);
  XXTact t = (XXTact){i, randIntBelow(0x40000000)};
  p->rent.nick = t.n;
  if (cash == 0) { //God
    numGods++;
    p->rent.cash = 0;
    p->rent.lastPaidRent = tocksNow();
    if (stuff) stuff(&p->body);
    p->rent.bomb = badXXBombIx;
    pileOfXXs.modUsr(1);
  } else {
    p->rent.cash = cash;
    p->rent.lastPaidRent = tocksNow();
    if (stuff) stuff(&p->body);
    Tocks expiry = p->rent.lastPaidRent + cash / (billableXXSize * tockPrice());
    meapOfXXBombs.insert(expiry, i.i, &p->rent.bomb); // Do we need the return value?
  }
  if (pp) *pp = p;
  return t;
}


// Assumes some mutex is held, despite the name
// That's true because onNewXX only called from meap's insert
//   which for the hotel is only called from hotel's admit
// We know it exists, and it doesn't have or need money
void onNewXXBomb(XXBombIx iBomb, Ix hint) {
  XXBomb * pBomb = pileOfXXBombs.get(iBomb);
  pBomb->who = (XXIx){hint};
//  XX * p = pileOfXXs.get(pBomb->who);
//  p->rent.bomb = iBomb;
//  updateDeathWithXXAndBomb_(p, pBomb);
//  meapOfXXBombs.check();
}

// Similarly thread safe already, I think?
void onMoveXXBomb(XXBomb * pBomb, XXBombIx to) {
  XX * p = pileOfXXs.get(pBomb->who);
  p->rent.bomb = to;
  meapOfXXBombs.check();
}

static void forAll(bool u, XXVIP act) { 
  pileOfXXs.forAll(false, act); 
}

XXHotel hotelOfXXs = { open, admit, get, with, 
  richer, poorer, collectRent, forAll, raid, 
  count, checkHotel, close, show, showXX };


const size_t billableXXSize = sizeof(XX)+sizeof(XXBomb);
