
typedef struct {
  int effort; // Old thing for testing
} PhyA;

typedef struct {
  float oddsLazyByCash0;
  float oddsLazyByCash1; // Hope it learns to repro only when rich
  Cash msgCashByCash0; //How much of cash goes to msg
  Cash msgCashByCash1; //  For child msg use params after mutation
  // Children start with same cash as Adam. 
} PhyB;

typedef  union {
  PhyA a;
  PhyB b;
} PhyData;

typedef struct {
  char phylum;
  PhyData p;
} MobBody;

void showMobBody(MobBody * p);

