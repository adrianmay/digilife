// #include <stdio.h>
#define _GNU_SOURCE
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <signal.h>
#include "h.h"

static int perf_event_open(struct perf_event_attr *attr, pid_t tid) {
  return syscall(__NR_perf_event_open, attr, tid, -1, -1, 0);
}

static int openCyclesEvent(int process, int disabled, Cycles sample_period) {
  struct perf_event_attr pe;
  memset(&pe, 0, sizeof(pe));
  pe.type = PERF_TYPE_HARDWARE;
  pe.size = sizeof(pe);
  pe.config = PERF_COUNT_HW_CPU_CYCLES;
  pe.exclude_kernel = 0;
  pe.exclude_hv = 1;
  pe.pinned = 0;
  pe.disabled = disabled;
  pe.inherit = process;
  if (sample_period) {
    pe.sample_period = sample_period;
    pe.wakeup_events = 1;
  }
  int fd = perf_event_open(&pe, 0); //(pid_t)syscall(SYS_gettid));
  if (fd < 0) { perror("perf_event_open timer"); exit(1); }
  return fd;
}

int processFd = -1;

void initProcessTimer() {
  processFd = openCyclesEvent(1, 0, 0);
}

void unitProcessTimer() {
  close(processFd);
}

typedef struct perf_event_mmap_page PerfMap; 

PerfMap * openPerfMap(int fd) {
  int pagesz = getpagesize();
  PerfMap * map = mmap(NULL, pagesz * 2,
      PROT_READ | PROT_WRITE,
      MAP_SHARED, fd, 0);
  if (map == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
  return map;
}

Timer initThreadTimer() {
  Timer t;
  t.fd = openCyclesEvent(0, 0, 1000000);
  t.map = openPerfMap(t.fd);
  return t;
}

void unitThreadTimer(Timer t) {
  int pagesz = getpagesize();
  munmap(t.map, pagesz * 2);
  close(t.fd);
}

Cycles readFd(int fd) {
  Cycles v;
  if (read(fd, &v, sizeof(v)) != sizeof(v)) {
    perror("read");
    exit(1);
  }
  return v;
}

Cycles readProcessCycles() {
  return readFd(processFd);
}

static inline uint64_t rdpmc(unsigned counter) {
  uint32_t lo, hi;
  __asm__ volatile("rdpmc" : "=a"(lo), "=d"(hi) : "c"(counter));
  return ((uint64_t)hi << 32) | lo;
}

Cycles readPerfMap(PerfMap * pMap) {
  uint32_t seq, idx;
  int64_t count;
  do {
    seq = __atomic_load_n(&pMap->lock, __ATOMIC_ACQUIRE);
    idx = pMap->index;
    count = pMap->offset;
    if (idx) { count += rdpmc(idx - 1); }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
  } while (__atomic_load_n(&pMap->lock, __ATOMIC_RELAXED) != seq);
  return count;
}

Cycles readThreadCycles(Timer t) {
  return readPerfMap(t.map);
}

Alarm * alarmsByFd[MAX_WORKER_THREADS];

void initThreadAlarm(Alarm * pA, PerfHandler h, PerfHandleC phc) {
  pA->handler = h;
  pA->phc = phc;
  pA->t.fd = openCyclesEvent(0, 1, 1000000);
  pA->t.map = openPerfMap(pA->t.fd);
  fcntl(pA->t.fd , F_SETFL, O_ASYNC | O_NONBLOCK);
  fcntl(pA->t.fd , F_SETSIG, SIGIO);
  fcntl(pA->t.fd , F_SETOWN, getpid());
  alarmsByFd[pA->t.fd] = pA;
}

void unitThreadAlarm(Alarm * pA) {
  unitThreadTimer(pA->t);
}

void setAlarm(Alarm * pA, Cycles cycles) {
  //Cycles cycles = cycles_ - 100000;
  int fd = pA->t.fd;
  ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
  ioctl(fd, PERF_EVENT_IOC_RESET, 0);
  if (!cycles) return;
  ioctl(fd, PERF_EVENT_IOC_PERIOD, &cycles);
  ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
}

Cycles readAlarmCycles(Alarm * pA) {
  return 0xFFFFFFFFFFFF & readThreadCycles(pA->t);
}

static void handler(int signo, siginfo_t *info, void *ucontext)
{
  Alarm * pAlarm = alarmsByFd[info->si_fd];
  ioctl(pAlarm->t.fd, PERF_EVENT_IOC_DISABLE, 0);
  pAlarm->handler(pAlarm->phc);
}

void installHandler() {
  struct sigaction sa = {0};
  sa.sa_sigaction = handler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGIO, &sa, NULL);
}

void initPerf() {
  installHandler();
  initProcessTimer();
}

void nsToTs(uint64_t ns, struct timespec * pTs) {
  memset(pTs, 0, sizeof(*pTs));
  lldiv_t qr = lldiv(ns, 1000000000);
  pTs->tv_sec = qr.quot;
  pTs->tv_nsec= qr.rem;
}

void sleepNs(uint64_t ns) {
  struct timespec ts;
  nsToTs(ns, &ts);
  clock_nanosleep(CLOCK_PROCESS_CPUTIME_ID, 0, &ts, 0);
}

