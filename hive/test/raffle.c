//     #include <unistd.h>
//     #include "types.h"
//     #include "test.h"
//     #include "misc/h.h"
//     #include "globals/h.h"
//     //#include "MessBulk_pile/1.h"
//     //#include "MessBomb_pile/1.h"
//     //#include "Mess_hotel/Bulk.h"
//     //#include "Mess_hotel/Bomb.h"
//     #include "Mess_hotel/h.h"
//     
//     static bool extinct = false;
//     void onMesssExtinct() { extinct = true; } 
//     
//     static bool init() {
//       openGlobals();
//       hotelOfMesss.open();
//       background(sweat_forever); // Got to do work to advance CPU time ...
//       return true;
//     }
//     
//     bool testRaffle() { return true; }
//     void cleanupRaffle() { closeGlobals(1); hotelOfMesss.close(1); }
//     bool raffle() { return bkt("raffle", init, testRaffle, cleanupRaffle); }


