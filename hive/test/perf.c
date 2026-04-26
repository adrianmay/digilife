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

static int perf_fd;

static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw_event, pid, cpu,
                   group_fd, flags);
}

long periods[] = {1000000000, 2000000000, 500000000};
int idx = 0;

void handler(int signo, siginfo_t *info, void *ucontext)
{
    printf("Iteration %d reached\n", idx);

    idx = (idx + 1) % 3;

    // stop counter
    ioctl(perf_fd, PERF_EVENT_IOC_DISABLE, 0);

    // set next period
    long next = periods[idx];
    ioctl(perf_fd, PERF_EVENT_IOC_PERIOD, &next);

    // reset + re-arm
    ioctl(perf_fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(perf_fd, PERF_EVENT_IOC_ENABLE, 0);
}

int perf()
{
    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(pe));

    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(pe);
    pe.config = PERF_COUNT_HW_CPU_CYCLES;

    pe.sample_period = 200000000; // threshold
    pe.disabled = 1;
    pe.exclude_kernel = 0;
    pe.exclude_hv = 1;
    pe.pinned = 1;

    pid_t pid = getpid(); //Whole process
    //pid_t pid = 0;        //This thread

    perf_fd = perf_event_open(&pe, pid, -1, -1, 0);
    if (perf_fd == -1) {
        perror("perf_event_open");
        exit(EXIT_FAILURE);
    }

    fcntl(perf_fd, F_SETFL, O_RDWR | O_NONBLOCK | O_ASYNC);
    fcntl(perf_fd, F_SETSIG, SIGIO);
    fcntl(perf_fd, F_SETOWN, getpid());

    struct sigaction sa = {0};
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGIO, &sa, NULL);

    ioctl(perf_fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(perf_fd, PERF_EVENT_IOC_REFRESH, 1);
    ioctl(perf_fd, PERF_EVENT_IOC_ENABLE, 0);

    while (1) {
        asm volatile("" ::: "memory");
    }

    return 0;
}

