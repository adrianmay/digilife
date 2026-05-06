#ifndef XXHotelBulk_h
#define XXHotelBulk_h

#include "XX_hotel/XXBody.h"
typedef struct {Cash cash; Tocks lastPaidRent; XXBombIndex bomb; } XXRent;
typedef struct {XXRent rent; XXBody body; } XXBulk;
// typedef struct {XXBulkIndex me; XXRent rent; XXBody body; } XXBulk;
 
#endif

