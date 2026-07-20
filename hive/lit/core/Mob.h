
typedef enum Phylum {
  PhyGod, PhyTest, PhyAnimal
} Phylum;

typedef struct GodMob { } GodMob;

typedef struct TestMob {
  Cash spawnThresh;
} TestMob;

typedef struct Mob {
  Phylum phylum;
  union {
    GodMob god;
    TestMob test;
  } _;
} Mob;


