#include "types.h"
#include "MobBulk_pile/1.h"
#include "MobBomb_pile/1.h"
#include "Bomb.h"
#include "Bulk.h"

Score getMobBombScore(MobBomb * pJ) { return pJ->when; }
void  onNewMobBomb(MobBombIndex iJ, Index hint) { }
void  onMoveMobBomb(MobBomb * pJ, MobBombIndex to) { }
