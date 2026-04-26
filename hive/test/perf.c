/////////////////////////////////////////////
////// test/perf.c

#define _GNU_SOURCE
#include <linux/perf_event.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include "types.h"
#include "misc/h.h"

int fd1;
int fd2;

long periods[] = {1000000000, 2000000000, 500000000};
int idx = 0;

void handler(int signo, siginfo_t *info, void *ucontext)
{
  printf("Iteration %d. told fd=%d pid=%d\n", idx, info->si_fd, info->si_value.sival_int);

  //    idx = (idx + 1) % 3;
  //    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
  //    long next = periods[idx];
  //    ioctl(fd, PERF_EVENT_IOC_PERIOD, &next);
  //    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
  //    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
}

bool init() {
  struct sigaction sa = {0};
  sa.sa_sigaction = handler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGIO, &sa, NULL);
  return true;
}

typedef struct {int secs; int fd;} go;

void * monitor(void * p)
{
  go * pGo = (go *) p;

  struct perf_event_attr pe;
  memset(&pe, 0, sizeof(pe));

  pe.type = PERF_TYPE_HARDWARE;
  pe.size = sizeof(pe);
  pe.config = PERF_COUNT_HW_CPU_CYCLES;

  pe.sample_period = 4000000000 * pGo->secs; // threshold
  pe.disabled = 1;
  pe.exclude_kernel = 0;
  pe.exclude_hv = 1;
  pe.pinned = 1;

  pid_t pid = true ? 0 : getpid(); //this thread (0) or cpu

  pGo->fd = syscall(__NR_perf_event_open, &pe, pid, -1, -1, 0);
  if (pGo->fd == -1) {
    perror("perf_event_open");
    exit(EXIT_FAILURE);
  }

  fcntl(pGo->fd, F_SETFL, O_RDWR | O_NONBLOCK | O_ASYNC);
  fcntl(pGo->fd, F_SETSIG, SIGIO);
  fcntl(pGo->fd, F_SETOWN, getpid());


  ioctl(pGo->fd, PERF_EVENT_IOC_RESET, 0);
  ioctl(pGo->fd, PERF_EVENT_IOC_REFRESH, 1);
  ioctl(pGo->fd, PERF_EVENT_IOC_ENABLE, 0);

  while (1) {
    asm volatile("" ::: "memory");
  }
}

go go1 = {2, 0};
go go2 = {4, 0};

bool perf() {
  init();
  pthread_t pid;
  pthread_create(&pid, 0, monitor, &go1);
  printf("Started thread: %ld\n", pid);
  pthread_create(&pid, 0, monitor, &go2);
  printf("Started thread: %ld\n", pid);
  pthread_join(pid, 0);
  return true;
}

// void * waitAndTalk(void * p) {
//   long i = (long) p;
//   Nanosecs ns = 1000000000 * i;
//   struct timespec ts;
//   nsToTs(ns, &ts);
//   printf("Should wait %ld s and %'ld ns\n", ts.tv_sec, ts.tv_nsec);
//   clock_nanosleep(CLOCK_THREAD_CPUTIME_ID, 0, &ts, 0);
//   printf("waited %lds\n", i);
//   return 0;
// }
// 
// bool perf1() {
//   pthread_t pid;
//   for (long a=0;a<10;a++) {pthread_create(&pid, 0, waitAndTalk, (void *)a);}
//   return true;
// }
