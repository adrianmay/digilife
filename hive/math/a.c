#include <stdio.h>
#include <stdlib.h>

#define LIMIT 2'000
#define STARTING_CASH 1'000'000
#define THRESH 20'000'000
#define EVES 5
#define RENT 1'000
#define DOLE 1'000'000
#define SPAWN_COST 5'000'000

#define MAX(a,b) ((a) > (b) ? (a) : (b))

#define SAMPLER(NAME, SPEED) \
  double NAME##Mean; \
  int NAME##Samples=0; \
  void NAME##Sample(double val) { \
    NAME##Samples++; \
    double speed = MAX(SPEED, 1.0/NAME##Samples); \
    NAME##Mean = speed*val + (1.0-speed)*NAME##Mean; \
  }

typedef long int Cash; 
Cash m[LIMIT]={0};
int pop;
Cash lost=0; 
long int its = 0;

SAMPLER(pop,   0.000000001)
SAMPLER(spawn, 0.000000001)

void die(int line) {
  printf("Dying at %d\n", line);
  abort();
}

int findEmpty() { 
  for (int a=0; a<LIMIT; a++) 
    if (!m[a]) 
      return a;
  die(__LINE__); 
}

void genesis() { 
  for (int a=0; a<EVES; a++) 
    m[a] = STARTING_CASH; 
  pop = EVES;
}

void collectRent() {
  for (int a=0; a<LIMIT; a++) {
    if (m[a]>0) {
      m[a] -= RENT;
      if (m[a]<=0) {
        pop--;
        lost -= m[a];
        m[a] = 0;
      }
    }
  }
}

void run() {
  int r;
  do {
    r = rand()%LIMIT;
  } while (!m[r]);
  m[r] += DOLE;
  Cash lostbit=lost/30;
  m[r] += lostbit;
  lost -= lostbit;
  collectRent();
  int spawned = 0;
  if (m[r] >= THRESH) {
    m[r] -= SPAWN_COST;
    Cash chCash = m[r] / 2;
    m[r] -= chCash;
    m[findEmpty()] = chCash;
    pop++;
    spawned = 1;
  }  
  if (its>1000000) 
    spawnSample(spawned);
}

int main() {
  genesis();
  for (its=0;;its++) {
    run();
    if (its>1000000)
      popSample(pop);
    if (its%10000 == 0)
      printf("Its=%'ld Pop=%f SpawnProb=%f Lost=%'ld\n", its, popMean, 1.0/spawnMean, lost);
  }
  return 0; 
}

// Its=208810000 Pop=975.191301 SpawnProb=201.505107 Lost=29
// Cos cash in per rep = 1m - 5m/201.5 = 975186

