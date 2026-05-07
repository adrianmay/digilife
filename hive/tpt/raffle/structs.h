// #pragma once

#include "types.h"
#include "bit/XXTicket.h"
// These are the self and subtree weights. The total weight is the sum of these:
typedef struct {Weight s; Weight l; Weight r;} XXRafle;
typedef struct {XXRafle raffle; XXTicket ticket;} XXBody;

