#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
//#include <pthread.h>
//#include <stdlib.h>
//#include <locale.h>
//#include <signal.h>
#include "assert.h"

typedef void (*V)();
typedef bool  (*B)();
typedef bool (*BV)();


bool bkt(const char * name, B up, B along, V down);
bool nowt();
void * sweat(void *);
void * sweat_forever(void *);
void background(void * (*f)(void *));

bool timer();
bool wrap();
bool pile();
bool meap();
bool globals();
bool rent();
bool hotel();

bool openGlobals();
void closeGlobals(bool);

