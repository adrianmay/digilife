#ifndef XXRaffle_h
#define XXRaffle_h

#include "XXTicket.h"
// These are subtree weights. The self weight is the difference of these:
typedef struct {Weight t; Weight l; Weight r;} XXRaffle;
typedef struct {XXRaffle raffle; XXTicket ticket;} XXBody;

#endif
