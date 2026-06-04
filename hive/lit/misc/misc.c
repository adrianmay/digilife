#include <sys/stat.h>
#include <unistd.h>
#include <sys/random.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "types.h"
#include "h.h"

////////////////////////////////////////////////////////////////

uint8_t  wrapAdd8  (uint8_t  a,  uint8_t b) { return a + b; }
uint32_t wrapAdd32 (uint32_t a, uint32_t b) { return a + b; }
uint64_t wrapAdd64 (uint64_t a, uint64_t b) { return a + b; }

uint8_t  wrapSub8U  (uint8_t  a, uint8_t  b) { return a - b; }
uint32_t wrapSub32U (uint32_t a, uint32_t b) { return a - b; }
uint64_t wrapSub64U (uint64_t a, uint64_t b) { return a - b; }

int8_t   wrapSub8S  (uint8_t  a, uint8_t  b) { return a - b; }
int32_t  wrapSub32S (uint32_t a, uint32_t b) { return a - b; }
int64_t  wrapSub64S (uint64_t a, uint64_t b) { return a - b; }

////////////////////////////////////////////////////////////////


int quit(int i) {
  abort();
} // { return *((int*)(0)); }

int fileSize(int fd) {
  struct stat sb;
  if (fstat(fd, &sb) == -1) { printf("Can't stat fd=%d\n", fd); quit(1); }
  return sb.st_size;
}

uint64_t randIntBelow(uint64_t lim) {
  uint64_t thresh = -1;
  thresh = -(thresh % lim + 1);
  uint64_t res;
  getrandom(&res, sizeof(res), 0);
//  printf("randIntBelow: lim=%lu, thresh=%lu, res=%lu\n", lim, thresh, res);
  if (res > thresh)
    return randIntBelow(lim); // Try again.
  else return res%lim;
  return res;
}

bool nearly(double a, double b, double f) {
  double m = (a+b)/2;
  double d2 = (b-m)*(m-a); // Positive
  double t = m*f;
  return d2 < t*t;
}

double gaussian_random(double mean, double stddev) {
  static int has_spare = 0;
  static double spare;
  if (has_spare) {
    has_spare = 0;
    return mean + stddev * spare;
  }
  has_spare = 1;
  double u, v, s;
  do {
    u = (rand() / ((double)RAND_MAX)) * 2.0 - 1.0;
    v = (rand() / ((double)RAND_MAX)) * 2.0 - 1.0;
    s = u * u + v * v;
  } while (s == 0 || s >= 1);
  s = sqrt(-2.0 * log(s) / s);
  spare = v * s;
  return mean + stddev * (u * s);
}

double clampProb(double p) {
  return MAX(0,MIN(1,p));
}
