// #include "test.h"
// #include "misc/h.h"
// 
// void * interrupter(void * p) {
//   pthread_t * pTid = (pthread_t*) p;
//   sleepS_(1);
//   wake(*pTid);
//   return 0;
// }
// 
// bool trysleep() {
//   printf("Starting sleep test at %'ld\n", ageOfTime());
//   Nanosecs ns = 2000000000;
//   pthread_t mtid, stid = sleepNs(&ns); // stid will sleep for 2s process cpu time
//   pthread_create(&mtid, 0, interrupter, &stid); // mtic will kill it after 1s
//   background(sweat_forever); // Got to do work to advance CPU time ...
//   background(sweat_forever); // ... at 2s per second.
//   wait(stid); // Wait til original timer exits
//   printf("Leaving sleep test at %'ld\n", ageOfTime()); // Expect it to have taken 0.5s of CPU process time
//   return true;
// }
// 
// bool sleep() { return trysleep(); }
// 
