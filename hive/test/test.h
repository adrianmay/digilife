#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
//#include <stdlib.h>
//#include <locale.h>
//#include <signal.h>
#include "assert.h"

typedef void (*V)(void);
typedef bool  (*B)(void);
typedef bool (*BV)(void);


bool bkt(const char * name, B up, B along, V down);
bool nowt(void);
void * sweat(void *);
void * sweat_forever(void *);
pthread_t background(void * (*f)(void *));

bool perf(void);
bool wrap(void);
bool pile(void);
bool meap(void);
bool globals(void);
bool rent(void);
bool hotel(void);
bool raffle(void);
bool work(void);
bool equi(void);

bool openGlobals(void);
void closeGlobals(bool);

