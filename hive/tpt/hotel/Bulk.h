#ifndef XXHotelBulk_h
#define XXHotelBulk_h

#include "XXBody.h"
typedef struct {Cash cash; Tocks lastPaidRent; XXBombIndex bomb; } XXRent;
typedef struct {XXBulkIndex me; XXRent rent; XXBody body; } XXBulk;
 
#endif

