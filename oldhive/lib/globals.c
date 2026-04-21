#include "globals.h"

/*
ECONOMICS

If this system is losing money it should sleep or archive itself.

Electricity should be covered by animals in proportion to their thread time usage.
Then the electricity used that can't be attributed to a specific animal must be measured and paid from tax.

Depreciation is incurred in proportion to calendar time, like buildings and flat online fees (but there'll also be per-byte internet fees later.) 
If the software isn't running, or is limited to half the CPU, that's not it's fault, so we use process CPU time as calendar time.

Labour is a placeholder for now. It gets much cheaper as the system scales, but behaves like calendar time.  

*/

const double cost_of_pc_in_pence = 150000;
const double life_of_pc_in_years = 3;
const double life_of_pc_in_seconds = life_of_pc_in_years*SECS_PER_YEAR;
const double depreciation_in_pence_per_second = cost_of_pc_in_pence/life_of_pc_in_seconds;
const double my_fee_per_hour_in_pence = 0.0;
const double my_time_proportion_on_digital_life = 0.05;
const double my_fee_in_pence_per_second = (my_fee_per_hour_in_pence/SECS_PER_HOUR)*my_time_proportion_on_digital_life;
const double other_calendar_stuff_in_pence_per_second = (3000 + 1000)/SECS_PER_MONTH; // Online flat rate and buildings
// In summary, per CPU process time:                                                                     
const double process_age_costs_in_pence_per_second = depreciation_in_pence_per_second 
                                                   + my_fee_in_pence_per_second 
                                                   + other_calendar_stuff_in_pence_per_second;
// So we must charge more rent that that.
                                                   
const double groats_per_penny = 1000000000; // So a groat is a nanopenny.
const double min_groats_per_nanosecond = process_age_costs_in_pence_per_second * groats_per_penny / NS_PER_SEC;
#define GUESS_NS_PER_TOCK 1000000                                           

//LATER:
// const double watts_per_busy_thread = 5;
// const double elec_price_in_pence_per_kWh = 50;
// const double elec_price_in_pence_per_joule = elec_price_in_pence_per_kWh/SECS_PER_HOUR/1000.0;
// // In summary:                                                                     
// const double thread_elec_cost_in_pence_per_second = watts_per_busy_thread * elec_price_in_pence_per_joule;  


void initVirginPersistentGlobals() {
  pg->lastKnownTock = 0;
  pg->nsNotTocked = 0;
  pg->nsPerTock = GUESS_NS_PER_TOCK; //Don't ignore animal for more than 2**32/nsPerTock
  pg->groatsPerTock = 1; //min_groats_per_nanosecond * GUESS_NS_PER_TOCK;
}

void initVolatileGlobals() {
  vg.tocksReviewedAt = 0;
  vg.shouldRun = true;
}

