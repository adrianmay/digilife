#include "types.h"
#include "XXBulk_pile/1.h"
#include "XXBomb_pile/1.h"
#include "Bomb.h"
#include "Bulk.h"

Score getXXBombScore(XXBomb * pJ) { return pJ->when; }
void  onNewXXBomb(XXBombIndex iJ, Index hint) { }
void  onMoveXXBomb(XXBomb * pJ, XXBombIndex to) { }
