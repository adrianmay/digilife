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


typedef struct {
  PerfHandler handler;
  PerfHandleC phc;
  pthread_t thread;
  int alarm_fd;
  int total_fd;
  void *map;
} Perf;

static Perf threadPerfsByFileHandle[MAX_WORKER_THREADS];

static int perf_event_open(struct perf_event_attr *attr, pid_t tid) {
  return syscall(__NR_perf_event_open, attr, tid, -1, -1, 0);
}

Cycles readAlarmCycles(PerfHandleS fd) {
  Cycles v;
  if (read(fd, &v, sizeof(v)) != sizeof(v)) {
    perror("read");
    exit(1);
  }
  return v;
}

Cycles readThreadCycles(PerfHandleS fd) {
  Perf * pPerf = &threadPerfsByFileHandle[fd];
  return readAlarmCycles(pPerf->total_fd);
}

void setAlarm(PerfHandleS fd, Cycles cycles_) {
  Cycles cycles = cycles_ - 100000;
  Perf * w = &threadPerfsByFileHandle[fd];
  ioctl(w->alarm_fd, PERF_EVENT_IOC_DISABLE, 0);
  ioctl(w->alarm_fd, PERF_EVENT_IOC_RESET, 0);
  ioctl(w->alarm_fd, PERF_EVENT_IOC_PERIOD, &cycles);
  ioctl(w->alarm_fd, PERF_EVENT_IOC_ENABLE, 0);
}

void cancelAlarm(PerfHandleS fd) {
  Perf * w = &threadPerfsByFileHandle[fd];
  ioctl(w->alarm_fd, PERF_EVENT_IOC_DISABLE, 0);
  ioctl(w->alarm_fd, PERF_EVENT_IOC_RESET, 0);
}

static void handler(int signo, siginfo_t *info, void *ucontext)
{
  Perf * pPerf = &threadPerfsByFileHandle[info->si_fd];
  ioctl(pPerf->alarm_fd, PERF_EVENT_IOC_DISABLE, 0);
  pPerf->handler(pPerf->phc);
}

void installHandler() {
  static int done = 0;
  if (done) return;
  struct sigaction sa = {0};
  sa.sa_sigaction = handler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGIO, &sa, NULL);
  done=1;
}

static int open_cycles_event(int countDescendentTasks, int disabled, Cycles sample_period) {
  struct perf_event_attr pe;
  memset(&pe, 0, sizeof(pe));
  pe.type = PERF_TYPE_HARDWARE;
  pe.size = sizeof(pe);
  pe.config = PERF_COUNT_HW_CPU_CYCLES;
  pe.disabled = disabled;
  pe.exclude_kernel = 0;
  pe.exclude_hv = 1;
  pe.pinned = 1;
  pe.inherit = countDescendentTasks;
  if (sample_period) {
    pe.sample_period = sample_period;
    pe.wakeup_events = 1;
  }
  return perf_event_open(&pe, (pid_t)syscall(SYS_gettid));
}

static PerfHandleS initCounter(int countDescendentTasks, PerfHandler ph, PerfHandleC phc) {
  int fd = open_cycles_event(countDescendentTasks, 1, 1000000);
  if (fd < 0) { perror("perf_event_open timer"); exit(1); }
  Perf * w = &threadPerfsByFileHandle[fd];
  w->handler = ph;
  w->phc = phc;
  w->alarm_fd = fd;
  int pagesz = getpagesize();
  if (!countDescendentTasks) {
    w->map = mmap(NULL, pagesz * 2,
        PROT_READ | PROT_WRITE,
        MAP_SHARED, w->alarm_fd, 0);
    if (w->map == MAP_FAILED) {
      perror("mmap");
      exit(1);
    }
  }
  fcntl(w->alarm_fd, F_SETFL, O_ASYNC | O_NONBLOCK);
  fcntl(w->alarm_fd, F_SETSIG, SIGIO);
  fcntl(w->alarm_fd, F_SETOWN, getpid());
  w->total_fd = open_cycles_event(countDescendentTasks, 0, 0);
  if (w->total_fd < 0) {
    perror("perf_event_open count");
    exit(1);
  }
  ioctl(w->total_fd, PERF_EVENT_IOC_RESET, 0);
  return fd;
}

PerfHandleS initThread(PerfHandler ph, PerfHandleC phc) {
  return initCounter(0, ph, phc);
}

PerfHandleS processCyclesHandleS;

void initPerf() {
  installHandler();
  processCyclesHandleS = initCounter(1, 0, 0);  // The handler never gets called
}

Cycles readProcessCycles() {
  return readThreadCycles(processCyclesHandleS);
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

