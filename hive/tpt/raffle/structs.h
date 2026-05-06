#ifndef XXRaffle_h
#define XXRaffle_h

#include "types.h"
#include "XXTicket.h"
// These are the self and subtree weights. The total weight is the sum of these:
typedef struct {Weight s; Weight l; Weight r;} XXRaffle;
typedef struct {XXRaffle raffle; XXTicket ticket;} XXBody;

#endif
