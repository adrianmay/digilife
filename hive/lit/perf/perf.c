// #include <stdio.h>
#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
//#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <signal.h>
#include "h.h"

uint64_t processCycles=0;

typedef struct {
  pthread_t pid;
  Cycles threadCyclesNow;
  PerfHandler handler;
  YourPerfHandle yph;
} ThreadPerf;

ThreadPerf threadPerfsByFileHandle[MAX_WORKER_THREADS];

Cycles readThreadCyclesNow(MyPerfHandle mph) {
  ThreadPerf * pTP = &threadPerfsByFileHandle[mph];
  size_t res = read(mph, &pTP->threadCyclesNow, sizeof(Cycles));
  if (res != sizeof(Cycles)) { printf("Dying in readThreadCyclesNow\n"); exit(1); }
  return pTP->threadCyclesNow;
}

static void handler(int signo, siginfo_t *info, void *ucontext) { }

void initOnce() {
  bool done = false;
  if (done) return;
  struct sigaction sa = {0};
  sa.sa_sigaction = handler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGIO, &sa, NULL);
  done=true;
}


MyPerfHandle initThreadPerf(PerfHandler ph, YourPerfHandle yph) {
  initOnce();
  struct perf_event_attr pe;
  memset(&pe, 0, sizeof(pe));
  pe.type = PERF_TYPE_HARDWARE;
  pe.size = sizeof(pe);
  pe.config = PERF_COUNT_HW_CPU_CYCLES;
  pe.inherit = 1;
  pe.disabled = 1;
  pe.exclude_kernel = 0;
  pe.exclude_hv = 1;
  pe.sample_type = PERF_SAMPLE_IP;
  pe.wakeup_events = 1;
  pe.sample_period = 10000000000;
  int fd = syscall(__NR_perf_event_open, &pe, 
                   0,  // This thread
                   -1, -1, 0);
  fcntl(fd, F_SETFL, O_RDWR | O_NONBLOCK | O_ASYNC);
  fcntl(fd, F_SETSIG, SIGIO);
  fcntl(fd, F_SETOWN, getpid());
  ThreadPerf * pTP = &threadPerfsByFileHandle[fd];
  pTP->handler = ph;
  pTP->yph     = yph;
  pTP->pid     = pthread_self();
  readThreadCyclesNow(fd);
  return fd;
}

void arm(MyPerfHandle mph, Cycles fromNow) {
}

Cycles threadJustUsed(MyPerfHandle mph) {
  return 0;
}
                                         
Cycles processCyclesNow() {
  return processCycles;
}
