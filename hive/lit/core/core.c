#include <setjmp.h>
#include "misc/h.h"
#include "globals/h.h"
#include "h.h"

#define PAY 2000

typedef struct {
//  pthread_t pid;
  Cycles used;
  Cycles limit;
  jmp_buf jmpbuf;
  int output;
} Core;

void stuffMsgPayload(MsgPayload * p) { (void)p; }

Cash mutCashThresh(Cash c) {
  double g = gaussian_random(1, 0.1);
  return c * g;
}

CpuBid mutCpuBid(CpuBid bid) {
  double g = gaussian_random(1, 0.1);
  return bid * g;
}

#define SLOWNESS 10

void burn(Core * pC, Cycles c, int line) {
  pC->used += SLOWNESS*c;
  //printf("burn: ")
  CycleDiff remaining = pC->limit - pC->used;
  if (remaining < 0) {
    //printf("Burning out from line %d\n", line);
    longjmp(pC->jmpbuf, pC->used);
  }
}

void runMob(Core * pC, Api api, MobTact tMe, 
            Cash mobCash, Cash msgCash, 
            MobBody * pMB, MsgTicket * pTicket) {
  //hotelOfMobs.check(1);
  if (pMB->phylum != PHY_B) abort();
  PhyB * pB = &pMB->p.b;
  //printf("runMob: mobcash=%ld, msgcash=%ld, thresh=%ld\n", mobCash, msgCash, pB->spawnThresh);
  mobPayMob(tSales, tMe, randIntBelow(PAY));
  burn(pC, 2, __LINE__);
  if ((mobCash) > pB->spawnThresh) {
    void stuffMobBody(MobBody * pCh) {
      pCh->phylum = PHY_B;
      PhyB * pCB = &pCh->p.b;
      pCB->spawnThresh = mutCashThresh(pB->spawnThresh);
      burn(pC, 2, __LINE__);
      pCB->payMsg = mutCashThresh(pB->payMsg);
      burn(pC, 2, __LINE__);
      pCB->bid = mutCpuBid(pB->bid);
      burn(pC, 1, __LINE__);
    }
    Cash forChild = (mobCash)/2 - pB->payMsg;
    if (forChild>0) {
      //hotelOfMobs.checkHotel(1);
      MobTact tCh = api.spawn(forChild, stuffMobBody);
      burn(pC, 3, __LINE__);
      api.post(pB->payMsg, pB->bid, tCh, stuffMsgPayload);
      burn(pC, 1, __LINE__);
    }
  }
  //MsgIx i = 
  api.post(pB->payMsg, pB->bid, tMe, stuffMsgPayload);
  //printf("Posted msg %d to mob %d\n", i.i, tMe.i.i);
  burn(pC, 1, __LINE__);
  //printf("End of runMob: \n");
  //raffleOfMsgs.show();
}

Burned run(Api api, MobTact tMe, Cash mobCash, Cash msgCash, MobBody * pMB, MsgTicket * pTicket) {
  Core core;
  core.used = core.output = 0;
  core.limit = msgCash / pTicket->cpuBid;
  int jmp = setjmp(core.jmpbuf);
  if (jmp == 0) { //Try
    runMob(&core, api, tMe, mobCash, msgCash, pMB, pTicket);
  } else {  //Catch
  }
  notifyCycles(core.used);
  if (core.used > core.limit)
    return (Burned){core.used, core.used - core.limit};
  else
    return (Burned){core.used, 0};
}
//     #include <stdatomic.h>
//     #include <math.h>
//     #include <string.h>
//     #include <setjmp.h>
//     #include <unistd.h>
//     #include "misc/h.h"
//     #include "args/h.h"
//     #include "perf/h.h"
//     //#include "Msg_raffle/h.h"
//     //#include "Mob_hotel/structs.h"
//     //#include "Mob_hotel/h.h"
//     #include "globals/h.h"
//     #include "tank/h.h"
//     #include "h.h"
//     
//     #define ITERS 100000000.0
//     #define START_MUT_RATE 2.0
//     #define MUT_RATE (START_MUT_RATE - ((double)iter) / ITERS )
//     #define MURDER_PER_MILLION 50
//     #define WEALTH_TAX 0.1
//     
//     
//     typedef struct Core {
//       pthread_t pid;
//       Cycles used;
//       Cycles limit;
//       jmp_buf jmpbuf;
//       int output;
//     } Core;
//     
//     typedef struct Env {
//     } Env;
//     
//     int iter=0;
//     
//     Weight bidToWeight(CpuBid bid) {return 10000000*bid;}
//     
//     double mutF(double p, double sd) {
//       double g = gaussian_random(0, sd);
//       return p+g;
//     }
//     
//     int mutI(int i, double sd) {
//       return round(mutF( (double)i, sd ));
//     }
//     
//     void burn(Core * pC, Cycles c) {
//       pC->used += c;
//       CycleDiff remaining = pC->limit - pC->used;
//       if (remaining < 0)
//         longjmp(pC->jmpbuf, pC->used);
//     }
//     
//     void runMobA(Core * pC, Mob * pMob, MobIx iMob, Env * pEnv) {
//       abort();
//     }
//     
//     void emit(Cash cash, CpuBid bid, MobTact tRcvr, MobTact tSndr) {
//       //if (cash>10000) { printf("Overrich 3 %d has %'ld\n", iR.i, cash); abort(); }
//       static MsgTicket tmp;
//       tmp.cpuBid = bid;
//       tmp.iRcvr = iR;
//       tmp.nRcvr = nRcvr;
//       hotelOfMobs.review(iS);
//       if (hotelOfMobs.chargeIfCan(iS, cash))
//         raffleOfMsgs.enter(cash, bidToWeight(bid), &tmp);
//     }
//     
//     void spawnB(Core * pC, MobIx iParent, Mob * pParent) {
//       //printf("Hoping to spawn... ");
//       //hotelOfMobs.showMob(iParent, pParent);
//       burn(pC, 50); //Spawn+selfmsg cost = 4*selfmsg alone
//       PhyB * pPB = &pParent->body.p.b;
//       Mob * pChild;
//       Cycles jobCycles = 1000000;
//       Cash jobCash = pPB->bid*jobCycles;
//       Cash nowCash = pParent->rent.cash;
//       Cash restCash = nowCash - jobCash;
//       if (restCash < 0) return;
//       burn(pC, 250); //Spawn+selfmsg cost = 4*selfmsg alone
//       Cash totChildCash = restCash/2;
//       Cash msgCash = totChildCash * pPB->fractionMsgOfMobSize / (1 + pPB->fractionMsgOfMobSize);
//       Cash childCash = totChildCash - msgCash;
//       //printf("Spawning...\n");
//       MobIx iChild = hotelOfMobs.alloc(childCash, &pChild, 0);
//       hotelOfMobs.enrich(iParent, -childCash);
//       PhyB * pCB = &pChild->body.p.b;
//       pChild->body.phylum = pParent->body.phylum;
//       pCB->pay = pPB->pay;
//       pCB->bid = pPB->bid;
//       pCB->fractionMsgOfMobSize = pPB->fractionMsgOfMobSize;
//       pCB->spawnThresh = mutI(pPB->spawnThresh,MUT_RATE);
//       emit(msgCash, pCB->bid, iChild, pChild->rent.nick, iParent);
//       hotelOfMobs.review(iParent);
//       hotelOfMobs.kill();
//     }
//     
//     void runMobB(Core * pC, Mob * pMob, MobIx iMob, Env * pEnv) {
//       //Maybe murder:
//       if (randIntBelow(1'000'000)<MURDER_PER_MILLION) {
//         hotelOfMobs.rob(iMob);
//         return;
//       }
//       PhyB * p = &pMob->body.p.b;
//       Cash cashNow = pMob->rent.cash;
//       hotelOfMobs.enrich(iMob, p->pay - WEALTH_TAX*cashNow); //Tax
//       cashNow = pMob->rent.cash;
//       bool spawn = cashNow > p->spawnThresh;
//       if (spawn) {
//         spawnB(pC, iMob, pMob);
//       }
//       burn(pC, 144);
//       emit(pMob->rent.cash * p->fractionMsgOfMobSize, p->bid, iMob, pMob->rent.nick, iMob);
//       if (0==iter%10) 
//         fprintf(outfile, "%d %d %d %d %ld %b\n"
//             , iter, hotelOfMobs.count(), raffleOfMsgs.count(), p->spawnThresh, cashNow, spawn);
//     }
//     
//     void runMob_(Core * pC, Mob * pMob, MobIx iMob, Env * pEnv) {
//       switch (pMob->body.phylum) {
//         case 'a':
//           runMobA(pC, pMob, iMob, pEnv);
//           break;
//         case 'b':
//           runMobB(pC, pMob, iMob, pEnv);
//           break;
//         default:
//           printf("Unknown phylum: %c\n", pMob->body.phylum);
//           exit(1);
//       }
//     }
//     
//     CycleDiff runMob(Core * pC, Mob * pMob, MobIx iMob, Env * pEnv, Cycles limit) {
//       pC->used = 0;
//       pC->limit = limit;
//       int jmp = setjmp(pC->jmpbuf);
//       int ret;
//       if (jmp==0) {
//         runMob_(pC, pMob, iMob, pEnv);
//         notifyCycles(pC->used);
//         ret = pC->used;
//       } else { // catch: jmp is total used
//         notifyCycles(jmp); // For counting total process cpu time
//         ret = limit-jmp; // Negative return value is overrun.
//       }
//       iter++;
//       return ret;
//     }
//     
//     
//     
//     
//     // int iter=0;
//     // FILE * outfile;
//     // 
//     // Core workers[MAX_WORKER_THREADS] = {0};
//     // Weight bidToWeight(CpuBid bid) {return 1000000000*bid;}
//     // 
//     // //void onMobsExtinct(void) { onXXsExtinct
//     // //  raffleOfMsgs.quit();
//     // //}
//     // 
//     // 
//     // //static void alarmHandler(void * p) {
//     // //  Worker * pW = (Worker *) p;
//     // //  pW->forceYield = true;
//     // //  //setAlarm(&pW->alarm, 0);
//     // //  pW->lastEnded = tocksNow();
//     // //  //printf("Job overran in %'ld\n", pW->lastEnded - pW->lastStarted);
//     // //}
//     // 
//     // void runMobA(Worker * pW, MobIx i, Mob * p) {
//     //   if (pW->output == 0) pW->output = 1;
//     //   if (p->body.p.a.effort == 10000001) pW->output += 10;
//     //   if (p->body.p.a.effort == 10000002) pW->output *= 2;
//     //   // So it's either 22 or 12 depending on the order.
//     //   int e = p->body.p.a.effort*0.264;
//     //   for (int x=0, a=0; a < e; a++) {
//     //     if (pW->forceYield) return;
//     //     x+=a;
//     //   }
//     //   if (pW->output == 1) pW->output=2;
//     // 
//     //   //printf("Just did Mob %d\n", i.i);
//     // }
//     // 
//     // 
//     // bool rollProb(double p) {
//     //   uint64_t m = 65536;
//     //   uint64_t t = m*p;
//     //   uint64_t r = randIntBelow(m);
//     //   return (r<t);
//     // }
//     // 
//     // double mutF(double p, double sd) {
//     //   double g = gaussian_random(0, sd);
//     //   return p+g;
//     // }
//     // 
//     // double mutScale(double p, double sd) {
//     //   double g = gaussian_random(0, sd*p);
//     //   return p+g;
//     // }
//     // 
//     // int mutI(int i, double sd) {
//     //   return round(mutF( (double)i, sd ));
//     // }
//     // 
//     // 
//     // static void burn(int effort) {
//     // 
//     // }
//     // 
//     // void runMobB(Worker * pW, MobIx i, Mob * pMob) {
//     //   if (randIntBelow(100000)<5) {
//     //     hotelOfMobs.rob(i);
//     //     return;
//     //   }
//     //   PhyB * p = &pMob->body.p.b;
//     //   Cash bestCash = pMob->rent.cash;
//     //   hotelOfMobs.enrich(i, p->pay - 0.1*bestCash);
//     //   bestCash = pMob->rent.cash;
//     //   bool spawn = bestCash > p->spawnThresh;
//     //   if (spawn) {
//     //     spawnB(i, pMob);
//     //     burn(300); //Spawn+selfmsg cost = 4*selfmsg alone
//     //   }
//     //   burn(144);
//     //   emit(pMob->rent.cash * p->fractionMsgOfMobSize, p->bid, i, pMob->rent.nick, i);
//     //   //if (0==iter%10) 
//     //     fprintf(outfile, "%d %d %d %d %ld %b\n"
//     //                   , iter, hotelOfMobs.count(), raffleOfMsgs.count(), p->spawnThresh, bestCash, spawn);
//     //   iter++;
//     // }
//     // 
//     // void runMob(Worker * pW, MobIx i, Mob * p) {
//     //   hotelOfMobs.review(i);
//     //   hotelOfMobs.kill();
//     //   switch (p->body.phylum) {
//     //     case 'a':
//     //       runMobA(pW, i, p);
//     //       break;
//     //     case 'b':
//     //       runMobB(pW, i, p);
//     //       break;
//     //     default:
//     //       printf("Unknown phylum: %c\n", p->body.phylum);
//     //       exit(1);
//     //   }
//     // }
//     // 
//     // void * workerThread(void * p) {
//     //   outfile = fopen("out","w");
//     //   Worker * pW = (Worker *) p;
//     //   memset(pW, 0, sizeof(Worker));
//     //   //pW->timer = initThreadTimer();
//     //   pW->pid = pthread_self();
//     //   //initThreadAlarm(&pW->alarm, alarmHandler, pW);
//     //   while (iter<ITERS) {
//     //     raffleOfMsgs.kill();
//     //     MsgTicket ticket;
//     //     Cash cash;
//     //     //raffleOfMsgs.show();
//     //     if (!raffleOfMsgs.draw(&ticket, &cash)) {
//     //       break;
//     //     }
//     //     //if (cash>10000) { printf("Overrich 4 %d has %'ld\n", ticket.iRcvr.i, cash); abort(); }
//     //     Cycles cycleLimit = cash / ticket.cpuBid; //TODO: careful
//     //     //printf("cycleLimit=%'ld, cash = %ld, cpuBid = %f\n", cycleLimit, cash, ticket.cpuBid);
//     //     setAlarm(&pW->alarm, cycleLimit);
//     //     pW->lastStarted = readThreadCycles(pW->timer);
//     //     if (pW->firstStarted == 0) pW->firstStarted = pW->lastStarted;
//     //     Mob * pMob = hotelOfMobs.get(ticket.iRcvr);
//     //     if (pMob->rent.nick & 0x80000000 || pMob->rent.nick != ticket.nRcvr) 
//     //       continue;
//     //     runMob(pW, ticket.iRcvr, pMob);
//     //     if (pW->forceYield) {
//     //       pW->forceYield = false;
//     //       //printf("FORCEYIELD\n");
//     //       hotelOfMobs.enrich(ticket.iRcvr, -25);
//     //     } else {
//     //       hotelOfMobs.enrich(ticket.iRcvr, 25);
//     //       pW->lastEnded = readThreadCycles(pW->timer);
//     //       CycleDiff used = pW->lastEnded - pW->lastStarted;
//     //       //if (cash>10000) { printf("Overrich 5 %d has %'ld\n", ticket.iRcvr.i, cash); abort(); }
//     //       cash -= used * ticket.cpuBid;
//     //       ////if (cash>10000) { printf("Overrich 6 %d has %'ld; used=%'ld, lastEnded=%'ld, lastStarted=%'ld\n", ticket.iRcvr.i, cash, used, pW->lastEnded, pW->lastStarted); abort(); }
//     //       hotelOfMobs.enrich(ticket.iRcvr, cash);
//     //      tpt/hotel/hotel.c cash = hotelOfMobs.get(ticket.iRcvr)->rent.cash;
//     //       //printf("\tTook: %'ld - %'ld = %'ld cycles with change = %'ld\n", pW->lastEnded, pW->lastStarted, used, cash);
//     //     }
//     //     setAlarm(&pW->alarm, 0);
//     //     // Get output and do stuff.
//     //   }
//     //   unitThreadAlarm(&pW->alarm);
//     //   unitThreadTimer(pW->timer);
//     //   fclose(outfile);
//     //   return 0;
//     // }
//     // 
//     // Worker * thisWorker(int w) {
//     //   return &workers[w];
//     // }
//     // 
//     // int runWorker(Worker * pW) {
//     //   workerThread(pW);
//     //   return pW->output;
//     // }
//     // // Assume files open
//     // void initWorkers() {
//     //   //Start the workers
//     //   for (int w=0;w<getNumWorkers();w++) {
//     //     pthread_t junk;
//     //     pthread_create(&junk, 0, workerThread, &workers[w]);
//     //   }
//     // }
//     // 
//     // void waitWorkersAllDone() {
//     //   for (int w=0;w<getNumWorkers();w++) {
//     //     if (workers[w].pid)
//     //       pthread_join(workers[w].pid,0);
//     //   }
//     // }
