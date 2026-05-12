// #include <stdio.h>
#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
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
  PerfHandleC phc;
} Perf;

Perf threadPerfsByFileHandle[MAX_WORKER_THREADS];

Cycles readCyclesNow(PerfHandleS phs) {
  Perf * pTP = &threadPerfsByFileHandle[phs];
  size_t res = read(phs, &pTP->threadCyclesNow, sizeof(Cycles));
  if (res != sizeof(Cycles)) { printf("Dying in readCyclesNow\n"); exit(1); }
  return pTP->threadCyclesNow;
}

static void handler(int signo, siginfo_t *info, void *ucontext) {
  Perf * pPerf = &threadPerfsByFileHandle[info->si_fd];
  pPerf->handler(pPerf->phc);
}

void initOnce() {
  bool done = false;
  if (done) return;
  struct sigaction sa = {0};
  sa.sa_sigaction = handler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGIO, &sa, NULL);
  done=true;
}

PerfHandleS initPerf(int countDescendentTasks, PerfHandler ph, PerfHandleC phc) {
  initOnce();
  struct perf_event_attr pe;
  memset(&pe, 0, sizeof(pe));
  pe.type = PERF_TYPE_HARDWARE;
  pe.size = sizeof(pe);
  pe.config = PERF_COUNT_HW_CPU_CYCLES;
  pe.inherit = countDescendentTasks;
  pe.disabled = 1;
  pe.exclude_kernel = 0;
  pe.exclude_hv = 1;
  pe.sample_type = PERF_SAMPLE_IP;
  pe.wakeup_events = 1;
  pe.sample_period = 1000000000000ull;
  int fd = syscall(__NR_perf_event_open, &pe, 
                   0,  // This thread
                   //gettid(),  // This thread
                   -1, -1, 0);
  fcntl(fd, F_SETFL, O_RDWR | O_NONBLOCK | O_ASYNC);
  fcntl(fd, F_SETSIG, SIGIO);
  fcntl(fd, F_SETOWN, getpid());
  Perf * pTP = &threadPerfsByFileHandle[fd];
  pTP->handler = ph;
  pTP->phc     = phc;
  pTP->pid     = pthread_self();
  ioctl(fd, PERF_EVENT_IOC_ENABLE);
  //ioctl(fd, PERF_EVENT_IOC_DISABLE);
  readCyclesNow(fd);
  return fd;
}

PerfHandleS initProcPerf(PerfHandler ph, PerfHandleC phc) {
  return initPerf(1, ph, phc);
}

PerfHandleS initThreadPerf(PerfHandler ph, PerfHandleC phc) {
  return initPerf(0, ph, phc);
}

void arm(PerfHandleS phs, Cycles fromNow) {
  ioctl(phs, PERF_EVENT_IOC_PERIOD, fromNow);
  ioctl(phs, PERF_EVENT_IOC_RESET, 0);
  ioctl(phs, PERF_EVENT_IOC_ENABLE, 0);
}

void disarm(PerfHandleS phs) {
  ioctl(phs, PERF_EVENT_IOC_PERIOD, 0);
  ioctl(phs, PERF_EVENT_IOC_RESET, 0);
  ioctl(phs, PERF_EVENT_IOC_DISABLE, 0);
}

Cycles threadJustUsed(PerfHandleS phs) {
  return 0;
}
                                         
Cycles processCyclesNow() {
  return processCycles;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

/*

#define CLOCK_SPEED 4000000000
#define MAX_FD 10
long periods[] = {1, 2, 4};

int state[MAX_FD] = {0,0,0,0,1,0};
pthread_t pidsByFd[MAX_FD]={0};

void armMonitor(int fd, long * cycles) {
  ioctl(fd, PERF_EVENT_IOC_PERIOD, cycles);
  ioctl(fd, PERF_EVENT_IOC_RESET, 0);
  ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
}

void testArm(int fd) {
  state[fd] = (state[fd] + 1) % 3;
  printf("State=%d fd=%d pid=%ld\n", state[fd], fd, pidsByFd[fd]);
  long next = periods[state[fd]]*CLOCK_SPEED;
  armMonitor(fd, &next);
}

void handler(int signo, siginfo_t *info, void *ucontext) { testArm(info->si_fd); }

static bool init(void) {
  struct sigaction sa = {0};
  sa.sa_sigaction = handler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGIO, &sa, NULL);
  return true;
}

int makeThreadMonitor(void) {
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
  pe.sample_period = CLOCK_SPEED/10.0; // dummy threshold

  int fd = syscall(__NR_perf_event_open, &pe, 
                   0,  // This thread
                   -1, -1, 0);
  if (fd == -1) {
    perror("perf_event_open");
    exit(EXIT_FAILURE);
  }

  fcntl(fd, F_SETFL, O_RDWR | O_NONBLOCK | O_ASYNC);
  fcntl(fd, F_SETSIG, SIGIO);
  fcntl(fd, F_SETOWN, getpid());
  return fd;
}

void burn(void) {
  while (1) {
    asm volatile("" ::: "memory");
  }
}

void * testThread(void * p) {
  //long which = (long) p;
  int fd = makeThreadMonitor();
  pidsByFd[fd]=pthread_self();
  testArm(fd);
  burn();
  return 0;
}

bool perf(void) {
  init();
  pthread_t pid1, pid2;
  pthread_create(&pid1, 0, testThread, (void*)0);
  printf("Started thread: %ld\n", pid1);
  pthread_create(&pid2, 0, testThread, (void*)1);
  printf("Started thread: %ld\n", pid2);
  pthread_join(pid1, 0);
  pthread_join(pid2, 0);
  return true;
}

*/

