

typedef enum Phylum {
  PhyGod, PhyMortal
} Phylum;

typedef struct   GodMob { }   GodMob;
typedef struct MortalMob {
  Cash spawnThresh;
  unsigned char code[MOB_BODY_SIZE - 16]; //Cash=8, Phylum=4, aligned to 16
} MortalMob;

typedef struct Mob {
  Phylum phylum;
  union {
    GodMob god;
    MortalMob mortal;
  } _;
} Mob;


