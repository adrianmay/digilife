#include "types.h"

#define SET 1
#define QUIT 2
#define WAIT 4
typedef int (*Worker)(Nanosecs * pNsRel); // Returns the above flags

void initXXTimer();
void workOnXXTimer(Worker worker);               //Works within the mutex and optionally sets the timer.
void loopOnXXTimer(Worker worker, Nanosecs max); //Works within the mutex and optionally quits, sets the timer, and/or waits on it.
void unitXXTimer();
