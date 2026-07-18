#include "types.h"

// This is quite inaccurate. We could do with a kernel module to cut out the middleman

// typedef struct perf_event_mmap_page PerfMap;
// 
// typedef struct {
//   int fd;
//   PerfMap *map;
// } Timer;
// 
// void initProcessTimer();
// Cycles readProcessCycles();
// 
// Timer initThreadTimer();
// void unitThreadTimer(Timer t);
// Cycles readThreadCycles(Timer t);
// 
// typedef void * PerfHandleC;
// typedef void (*PerfHandler)(PerfHandleC phc);
// 
// typedef struct {
//   PerfHandler handler;
//   PerfHandleC phc;
//   Timer t;
// } Alarm;
// 
// void initThreadAlarm(Alarm * pA, PerfHandler h, PerfHandleC phc);
// void unitThreadAlarm(Alarm * pA);
// void setAlarm(Alarm * pA, Cycles cycles_);
// Cycles readAlarmCycles(Alarm * pA);
// 
// void initPerf(); // Call this early in the whole process from the main thread

void nsToTs(uint64_t ns, struct timespec * pTs);
void sleepNs(uint64_t ns);
void sleepMs(uint64_t ms);

