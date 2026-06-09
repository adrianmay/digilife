// #pragma once

#include "types.h"
#include "YY"
// These are the self and subtree weights. The total weight is the sum of these:
typedef struct {Weight s; Weight l; Weight r;} XXRafle;
typedef struct {XXRafle raffle; XXTicket ticket; } XXBody;

void showXXBody(XXIx i, XXBody * p); //Add index param so we can show pointers to lchild, rchild, parent in raffle
