#include <stdint.h>

#define KILO 1024
#define MEGA (KILO*KILO)
#define GIGA (MEGA*KILO)
#define TERA (GIGA*KILO)
#define B8 256ull
#define B16 (B8*B8)
#define B32 (B16*B16)

define(`PONDER_PILE',`typedef struct { uint32_t i; } $1Index;')dnl
dnl 
// Simple fixed-record-size heap 
define(`DECL_PILE',`
typedef struct {
$2} $1;
dnl
bool openPileOf$1s(bool forgetful);
void closePileOf$1s(bool delete);
Pilehead * pileheadOf$1s();
bool openForgetfulPileOf$1s();
$1 * get$1($1Index);
$1Index alloc$1($1Ghost * g);
void free$1($1Index, $1Ghost *);
bool isBad$1Index($1Index);
bool isSame$1Index($1Index, $1Index);
extern const $1Index zeroth$1Index;
extern const $1Index first$1Index;
extern const $1Index    bad$1Index;
typedef void (*walkerOf$1s)($1Index a, $1* p);
void walk$1s(walkerOf$1s w);
void trace$1(char * buf, $1 *);
uint32_t countFree$1s();
uint32_t countPop$1s();
uint32_t getUsr$1();
void setUsr$1(uint32_t u);
void modUsr$1(int32_t u);
uint32_t topOf$1s();
extern $1 * arrayOf$1s;
extern $1 prototype$1;')dnl
dnl
define(`IMPL_PILE',`
uint32_t      stepOf$1() { return $3; }
uint64_t     limitOf$1() { return $2; }
uint32_t  preambleOf$1() { return sizeof(Pilehead); }
$1 * arrayOf$1s;
Pilehead * pileheadOf$1s() { return ((Pilehead*)arrayOf$1s)-1; }
bool isBad$1Index ($1Index i) {return BAD_INDEX == i.i; }
bool isSame$1Index($1Index a, $1Index b) { return a.i==b.i; }
bool openPileOf$1s (bool forgetful) {
  arrayOf$1s = ($1 *) openPileInternal(forgetful?"":"$1.pile", sizeof($1), preambleOf$1(), stepOf$1(), limitOf$1());
  return pileheadOf$1s() -> top == 0;
}
void closePileOf$1s(bool delete) { closeInternal(pileheadOf$1s(), delete?"$1.pile":0); }
$1 * get$1 ($1Index i) { return arrayOf$1s + i.i; }
$1Index alloc$1 ($1Ghost * g) {
  $1Index i = { allocInternal( pileheadOf$1s(), sizeof($1), preambleOf$1(), stepOf$1(), 
                               (char*) &prototype$1, (void*)g, sizeof($1Ghost)  ) };
  return i;
}
void free$1 ($1Index i, $1Ghost * g) {
  freeInternal(pileheadOf$1s(), i.i, sizeof($1), preambleOf$1(), (void*)g, sizeof($1Ghost));
}
void walk$1s(walkerOf$1s w) { // How does the user know it aint free? Cos its stuffed with 0xAA but for the first 32bits plus ghost. The user can initialise a byte to 0 in the prototype, leave it alone, and test it for AA.
  for (int a=0;a<pileheadOf$1s()->top;a++) {
    $1Index i = {a};
    w(i, get$1(i));
  }
}
uint32_t topOf$1s() { return pileheadOf$1s()->top; }
uint32_t countFree$1s() { return countFree(pileheadOf$1s(), sizeof($1)); }
uint32_t countPop$1s() { return countPop(pileheadOf$1s(), sizeof($1)); }
uint32_t getUsr$1() { return getUsr(pileheadOf$1s()); }
void setUsr$1(uint32_t u) { setUsr(pileheadOf$1s(), u); }
void modUsr$1(int32_t u) { modUsr(pileheadOf$1s(), u); }
const $1Index zeroth$1Index = {0};
const $1Index  first$1Index = {1};
const $1Index    bad$1Index = {BAD_INDEX};
')dnl
dnl
define(`PONDER_LIST',
`PONDER_PILE($1)dnl
PONDER_PILE($1Node)dnl
typedef $1NodeIndex $1List;')dnl
define(`DECL_LIST',`dnl
DECL_PILE($1, $2)dnl
DECL_PILE($1Node,`dnl
  $1NodeIndex l;
  $1NodeIndex r;
  $1Index b;
')dnl
void append$1($1NodeIndex * piHead, $1Index body);
void rotate$1sBack($1List * piHead);
void prepend$1($1List * piHead, $1Index body);
typedef void * (*iteratorOf$1s)($1Index, void*);
void iterate$1s($1List iHead, iteratorOf$1s cb, void*);
')dnl
define(`IMPL_LIST',
`IMPL_PILE($1,$2,$3)
IMPL_PILE($1Node,$2,$3)
$1Node prototype$1Node = {BAD_INDEX, BAD_INDEX, BAD_INDEX};
void append$1($1List * piHead, $1Index body) {
  $1NodeIndex iNode = alloc$1Node(0);
  $1Node * pNode = get$1Node(iNode);
  if (isBad$1NodeIndex(*piHead)) {
    *piHead = pNode->l = pNode->r = iNode;
  } else {
    $1Node * pHead = get$1Node(*piHead);
    $1Node * pTail = get$1Node(pHead->l);
    pNode->r = *piHead;
    pNode->l = pHead->l;
    pHead->l = iNode;
    pTail->r = iNode;
  }
}
void rotate$1sBack($1List * piHead) { *piHead = get$1Node(*piHead)->l; }
void prepend$1($1List * piHead, $1Index body) {
  append$1(piHead, body);
  rotate$1sBack(piHead);
}
void iterate$1s($1List iHead, iteratorOf$1s cb, void * user) {
  if (!isBad$1NodeIndex(iHead)) {
    $1NodeIndex i = iHead;
    do {
      $1Node * pN = get$1Node(i);
      cb(pN->b, user);
      i = pN->r;
    } while (!isSame$1NodeIndex(i,iHead));
  }
}
')dnl
dnl
define(`PONDER_TREE',`PONDER_LIST($1)')dnl
define(`DECL_TREE',`
DECL_LIST($1,`dnl
  $1List sups;
  $1List subs;
$2')
typedef void * (*traverserOf$1s)($1Index i, $1Index iSup, void * u);
typedef void (*indenterOf$1s)(bool, void * u);
void traverse$1s($1Index i, $1Index iSup, traverserOf$1s cbT, indenterOf$1s cbI, void * u);
void  append$1InTree($1Index iNew, $1Index iSup);
void prepend$1InTree($1Index iNew, $1Index iSup);
const char * nameOf$1($1 * pT);
$1Index find$1InTree($1Index iRoot, const char * path);
$1Index find$1InTreeTop($1Index iRoot, const char * path);
')dnl
define(`IMPL_TREE',`
IMPL_LIST($1,$2,$3)
void traverse$1s($1Index iNode, $1Index iSup, traverserOf$1s cbT, indenterOf$1s cbI, void * u){
  void * u2 = cbT(iNode, iSup, u);
  $1 * pNode = get$1(iNode);
  if (!isBad$1NodeIndex(pNode->subs)) {
    cbI(true, u);
    $1NodeIndex i = pNode->subs;
    do {
      $1Node * pN = get$1Node(i);
      traverse$1s(pN->b, iNode, cbT, cbI, u2);
      i = pN->r;
    } while (!isSame$1NodeIndex(i,pNode->subs));
    cbI(false, u);
  }
}
typedef struct {jmp_buf jb; $1Index i; const char * path; int seglen; } Finding$1;
void * examine$1($1Index i, void * u) {
  Finding$1 * f = (Finding$1*) u;
  f->i = i;
  $1 * p = get$1(i);
  if (f->seglen==strlen(nameOf$1(p)) && 0==strncmp(f->path, nameOf$1(p), f->seglen))
    longjmp(f->jb,1);
  return u;
}
$1Index findSub$1($1Index iRoot, const char * path, int seglen) {
  Finding$1 f;
  f.path = path;
  f.seglen=seglen;
  $1 * pRoot = get$1(iRoot);
  int j;
  if ((j=setjmp(f.jb))) return f.i;
  iterate$1s(pRoot->subs, examine$1, &f);
  return bad$1Index;
}
$1Index find$1InTree($1Index iRoot, const char * path) {
  char * dot = strchr(path, SEPARATOR);
  int seglen = dot ? dot-path : strlen(path);
  $1Index i = findSub$1(iRoot, path, seglen);
  if (!dot || isBad$1Index(i)) return i;
  return find$1InTree(i, dot+1);
}
$1Index find$1InTreeTop($1Index iRoot, const char * path) {
  char * dot = strchr(path, SEPARATOR);
  int seglen = dot ? dot-path : strlen(path);
  $1 * pRoot = get$1(iRoot);
  if (seglen==strlen(nameOf$1(pRoot)) && 0==strncmp(path, nameOf$1(pRoot), seglen)) {
    if (dot)
      return find$1InTree(iRoot, dot+1);
    else
      return iRoot;
  }
  else
    return bad$1Index;
}
void append$1InTree($1Index iNew, $1Index iSup) {
  $1 * pNew = get$1(iNew);
  append$1(&pNew->sups, iSup);
  if (isBad$1Index(iSup)) return;
  $1 * pSup = get$1(iSup);
  append$1(&pSup->subs, iNew);
}
void prepend$1InTree($1Index iNew, $1Index iSup) {
  $1 * pNew = get$1(iNew);
  prepend$1(&pNew->sups, iSup);
  if (isBad$1Index(iSup)) return;
  $1 * pSup = get$1(iSup);
  prepend$1(&pSup->subs, iNew);
  prepend$1(&pNew->sups, iSup);
  if (isBad$1Index(iSup)) return;
}
')dnl
dnl
define(`PONDER_LOOK', `
PONDER_PILE($1To$2)dnl
')dnl
define(`DECL_LOOK',`
DECL_PILE($1To$2,`dnl
  $1 f;
  $2 t;
  $1To$2Index l;
  $1To$2Index g;'
)
dnl
bool greater$1($1*, $1*); // User to define
void enter$1To$2($1To$2Index *, $1*, $2*);
bool lookup$1To$2($1To$2Index *, $1*, $2*);
')dnl
define(`IMPL_LOOK',`
IMPL_PILE($1To$2,$3,$4)
void enter_$1To$2($1To$2Index iRoot, $1 * f, $2 * t) {
  $1To$2 * pRoot = get$1To$2(iRoot);
  if (0==memcmp(f, &pRoot->f, sizeof(pRoot->f))) {
    memcpy(&pRoot->t,t,sizeof(pRoot->t));
  } else {
    enter$1To$2(greater$1(f, &pRoot->f) ? &pRoot->g : &pRoot->l, f, t);
  }
}
void enter$1To$2($1To$2Index * piRoot, $1 * f, $2 * t) {
  if (isBad$1To$2Index(*piRoot)) {
    *piRoot = alloc$1To$2(0);
    $1To$2 * p = get$1To$2(*piRoot);
    memcpy(&p->f,f,sizeof(p->f));
    memcpy(&p->t,t,sizeof(p->t));
  } else enter_$1To$2(*piRoot, f, t);
}
bool lookup$1To$2($1To$2Index * piRoot, $1 * f, $2 * t) {
  if (isBad$1To$2Index(*piRoot)) return false;
  $1To$2 * pRoot = get$1To$2(*piRoot);
  if (0==memcmp(f, &pRoot->f, sizeof(pRoot->f))) { memcpy(t, &pRoot->t,sizeof(pRoot->t)); return true; }
  return lookup$1To$2(greater$1(f, &pRoot->f) ? &pRoot->g : &pRoot->l, f, t);
}
')dnl
dnl
dnl
typedef uint64_t MeapScore;
define(`PONDER_MEAP',
`PONDER_PILE($1Meap)
')dnl
define(`DECL_MEAP',
`DECL_PILE($1Meap,`dnl
  $1Index idx;
$2'
)
typedef void (* $1Modifier) ($1*);
extern MeapScore meapScoreOf$1($1Meap*); // provide this - lowest gets to root of tree
bool addTo$1Meap($1Index, MeapScore * newLowScore); // returns if low score changed
bool removeFrom$1Meap($1Index, MeapScore * newLowScore); // returns if low score changed
bool extractLowest$1InMeap();
bool extractBelow$1InMeap(MeapScore, $1*); // returns if more to come
void modify$1InMeap($1Modifier);
')dnl
define(`IMPL_MEAP',`dnl
IMPL_PILE($1Meap, $2, $3)

void swap$1Meap($1MeapIndex mi1, $1MeapIndex mi2) {
  $1Meap * pm1 = get$1Meap(mi1);
  $1Meap * pm2 = get$1Meap(mi2);
  $1 * p1 = get$1(pm1->idx);
  $1 * p2 = get$1(pm2->idx);
  $1Meap m;
  memcpy(&m , pm1, sizeof($1Meap));
  memcpy(pm1, pm2, sizeof($1Meap));
  memcpy(pm2, &m , sizeof($1Meap));
  p2->meap = mi1;
  p1->meap = mi2;
}

uint32_t meapParent(uint32_t i);
uint32_t meapLeft  (uint32_t i);
uint32_t meapRight (uint32_t i);

// Returns whether or not root changed
bool siftUp$1Meap($1MeapIndex miCur) {
  while (miCur.i>0) {
    $1Meap* pCur = get$1Meap(miCur);
    $1MeapIndex miParent = { meapParent(miCur.i) };
    $1Meap* pParent = get$1Meap(miParent);
    if (meapScoreOf$1(pParent) <= meapScoreOf$1(pCur))
      return false;
    swap$1Meap(miCur, miParent);  
    miCur = miParent;
  }
  return true;
}

void siftDown$1Meap($1MeapIndex miCur) {
  uint32_t numMeaps = countPop$1Meaps();
  while (1) {
    $1MeapIndex miLeft  = { meapLeft (miCur.i) };
    $1MeapIndex miRight = { meapRight(miCur.i) };
    $1MeapIndex miSmallest = miCur;
    uint64_t curScore = meapScoreOf$1(get$1Meap(miCur));
    if      (miLeft.i  < numMeaps && meapScoreOf$1(get$1Meap(miLeft )) < curScore) miSmallest = miLeft;
    else if (miRight.i < numMeaps && meapScoreOf$1(get$1Meap(miRight)) < curScore) miSmallest = miRight;
    if (miSmallest.i == miCur.i) break;
    swap$1Meap(miCur, miSmallest);
    miCur = miSmallest;
  }
}

// We cant have gaps in the meap so we cant use free.

bool addTo$1Meap($1Index i, MeapScore * newLowScore) {
  MeapScore nls;
  $1MeapIndex mi;
  uint32_t meapTop = getUsr$1Meap();
  if (meapTop < topOf$1Meaps())
    mi.i = meapTop;
  else
    mi = alloc$1Meap(0);
  modUsr$1Meap(1);

  $1Meap * pM = get$1Meap(mi);
  pM -> idx = i;
  $1 * p = get$1(i);
  p->meap = mi;
  return siftUp$1Meap(mi);
}

bool removeFrom$1Meap($1Index i, MeapScore * newLowScore) {
  MeapScore ms = meapScoreOf$1(get$1Meap(zeroth$1MeapIndex));
  $1 * p = get$1(i);
  $1MeapIndex mi = p->meap;
  $1MeapIndex li = { getUsr$1Meap() };
  swap$1Meap(mi, li);
  modUsr$1Meap(-1);
  siftDown$1Meap(mi);
  siftUp$1Meap(mi);
  *newLowScore = meapScoreOf$1(get$1Meap(zeroth$1MeapIndex));
  return (*newLowScore < ms);
}



')dnl
dnl
