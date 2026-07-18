#include <stdatomic.h>
#include "XXBomb_pile/ix.h"
#include "YY"

typedef struct XXNb   { Nick n; XXBombIx b; } XXNb; 
typedef union  XXInb  { _Atomic uint64_t i; XXNb nb; } XXInb;
typedef struct XXRent { XXInb inb; Tocks lastPaidRent; Cash cash; } XXRent;
typedef struct XXBlob { XXRent rent; XX body; } XXBlob;
