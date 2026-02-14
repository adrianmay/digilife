define(`PONDER_PILE',`typedef struct { uint32_t i; } '$1`Index;')dnl
 
// Simple fixed-record-size heap 
define(`DECL_PILE',`
typedef struct {
$2} '$1`;

bool openPileOf'$1`s(bool forgetful);
Pilehead * pileheadOf'$1`s();
bool openForgetfulPileOf'$1`s();
'$1` * get'$1`('$1`Index);
'$1`Index alloc'$1`('$1`Ghost * g);
void free'$1`('$1`Index, `$1'Ghost *);
bool isBad'$1`Index('$1`Index);
bool isSame'$1`Index('$1`Index, '$1`Index);
extern const '$1`Index zeroth'$1`Index;
extern const '$1`Index first'$1`Index;
extern const '$1`Index    bad'$1`Index;
typedef void (*walkerOf'$1`s)('$1`Index a, '$1`* p);
void walk'$1`s(walkerOf'$1`s w);
void trace'$1`(char * buf, '$1` *);
uint32_t countFree'$1`s();
uint32_t topOf'$1`s();
extern '$1` * arrayOf'$1`s;
extern '$1` prototype'$1`;')dnl

define(`IMPL_PILE',`
uint32_t      stepOf'$1`() { return '$3`; }
uint64_t     limitOf'$1`() { return '$2`; }
uint32_t  preambleOf'$1`() { return sizeof(Pilehead); }
'$1` * arrayOf'$1`s;
Pilehead * pileheadOf'$1`s() { return ((Pilehead*)arrayOf'$1`s)-1; }
bool isBad'$1`Index ('$1`Index i) {return BAD_INDEX == i.i; }
bool isSame'$1`Index('$1`Index a, '$1`Index b) { return a.i==b.i; }
bool openPileOf'$1`s (bool forgetful) {
  arrayOf'$1`s = ('$1` *) openPileInternal(forgetful?"":"'$1`.pile", sizeof('$1`), preambleOf'$1`(), stepOf'$1`(), limitOf'$1`());
  return pileheadOf'$1`s() -> top == 0;
}
'$1` * get'$1` ('$1`Index i) { return arrayOf'$1`s + i.i; }
'$1`Index alloc'$1` ('$1`Ghost * g) {
  '$1`Index i = { allocInternal(pileheadOf'$1`s(), sizeof('$1`), preambleOf'$1`(), stepOf'$1`(), (char*) &prototype'$1`, (void*)g, sizeof('$1`Ghost)) };
  return i;
}
void free'$1` ('$1`Index i, '$1`Ghost * g) {
  freeInternal(pileheadOf'$1`s(), i.i, sizeof('$1`), preambleOf'$1`(), (void*)g, sizeof('$1`Ghost));
}
void walk'$1`s(walkerOf'$1`s w) { // How does the user know it aint free? Cos its stuffed with 0xAA but for the first 32bits plus ghost. The user can initialise a byte to 0 in the prototype, leave it alone, and test it for AA.
  for (int a=0;a<pileheadOf'$1`s()->top;a++) {
    '$1`Index i = {a};
    w(i, get'$1`(i));
  }
}
uint32_t topOf'$1`s() { return pileheadOf'$1`s()->top; }
uint32_t countFree'$1`s() { return countFree(pileheadOf'$1`s(), sizeof('$1`)); }
const '$1`Index zeroth'$1`Index = {0};
const '$1`Index  first'$1`Index = {1};
const '$1`Index    bad'$1`Index = {BAD_INDEX};
')dnl

define(`PONDER_LIST',
`PONDER_PILE('$1`)
PONDER_PILE('$1`Node)
typedef '$1`NodeIndex '$1`List;')dnl
define(`DECL_LIST',`
DECL_PILE('$1`, '$2`)
DECL_PILE('$1`Node,
``  ''$1`NodeIndex l;
  '$1`NodeIndex r;
  '$1`Index b;
')dnl
void append'$1`('$1`NodeIndex * piHead, '$1`Index body);
void rotate'$1`sBack('$1`List * piHead);
void prepend'$1`('$1`List * piHead, '$1`Index body);
typedef void * (*iteratorOf'$1`s)('$1`Index, void*);
void iterate'$1`s('$1`List iHead, iteratorOf'$1`s cb, void*);
')dnl
define(`IMPL_LIST',
`IMPL_PILE('$1`,'$2`,'$3`)
IMPL_PILE('$1`Node,'$2`,'$3`)
'$1`Node prototype'$1`Node = {BAD_INDEX, BAD_INDEX, BAD_INDEX};
void append'$1`('$1`List * piHead, '$1`Index body) {
  '$1`NodeIndex iNode = alloc'$1`Node(0);
  '$1`Node * pNode = get'$1`Node(iNode);
  if (isBad'$1`NodeIndex(*piHead)) {
    *piHead = pNode->l = pNode->r = iNode;
  } else {
    '$1`Node * pHead = get'$1`Node(*piHead);
    '$1`Node * pTail = get'$1`Node(pHead->l);
    pNode->r = *piHead;
    pNode->l = pHead->l;
    pHead->l = iNode;
    pTail->r = iNode;
  }
}
void rotate'$1`sBack('$1`List * piHead) { *piHead = get'$1`Node(*piHead)->l; }
void prepend'$1`('$1`List * piHead, '$1`Index body) {
  append'$1`(piHead, body);
  rotate'$1`sBack(piHead);
}
void iterate'$1`s('$1`List iHead, iteratorOf'$1`s cb, void * user) {
  if (!isBad'$1`NodeIndex(iHead)) {
    '$1`NodeIndex i = iHead;
    do {
      '$1`Node * pN = get'$1`Node(i);
      cb(pN->b, user);
      i = pN->r;
    } while (!isSame'$1`NodeIndex(i,iHead));
  }
}
')dnl

define(`PONDER_TREE',`PONDER_LIST('$1`)')dnl
define(`DECL_TREE',`
DECL_LIST('$1`,
``  ''$1`List sups;
  '$1`List subs;dnl
'$2)
typedef void * (*traverserOf'$1`s)('$1`Index i, '$1`Index iSup, void * u);
typedef void (*indenterOf'$1`s)(bool, void * u);
void traverse'$1`s('$1`Index i, '$1`Index iSup, traverserOf'$1`s cbT, indenterOf'$1`s cbI, void * u);
void  append'$1`InTree('$1`Index iNew, '$1`Index iSup);
void prepend'$1`InTree('$1`Index iNew, '$1`Index iSup);
const char * nameOf'$1`('$1` * pT);
'$1`Index find'$1`InTree('$1`Index iRoot, const char * path);
'$1`Index find'$1`InTreeTop('$1`Index iRoot, const char * path);
')dnl
define(`IMPL_TREE',`
IMPL_LIST('$1`,'$2`,'$3`)
void traverse'$1`s('$1`Index iNode, '$1`Index iSup, traverserOf'$1`s cbT, indenterOf'$1`s cbI, void * u){
  void * u2 = cbT(iNode, iSup, u);
  '$1` * pNode = get'$1`(iNode);
  if (!isBad'$1`NodeIndex(pNode->subs)) {
    cbI(true, u);
    '$1`NodeIndex i = pNode->subs;
    do {
      '$1`Node * pN = get'$1`Node(i);
      traverse'$1`s(pN->b, iNode, cbT, cbI, u2);
      i = pN->r;
    } while (!isSame'$1`NodeIndex(i,pNode->subs));
    cbI(false, u);
  }
}
typedef struct {jmp_buf jb; '$1`Index i; const char * path; int seglen; } Finding'$1`;
void * examine'$1`('$1`Index i, void * u) {
  Finding'$1` * f = (Finding'$1`*) u;
  f->i = i;
  '$1` * p = get'$1`(i);
  if (f->seglen==strlen(nameOf'$1`(p)) && 0==strncmp(f->path, nameOf'$1`(p), f->seglen))
    longjmp(f->jb,1);
  return u;
}
'$1`Index findSub'$1`('$1`Index iRoot, const char * path, int seglen) {
  Finding'$1` f;
  f.path = path;
  f.seglen=seglen;
  '$1` * pRoot = get'$1`(iRoot);
  int j;
  if ((j=setjmp(f.jb))) return f.i;
  iterate'$1`s(pRoot->subs, examine'$1`, &f);
  return bad'$1`Index;
}
'$1`Index find'$1`InTree('$1`Index iRoot, const char * path) {
  char * dot = strchr(path, SEPARATOR);
  int seglen = dot ? dot-path : strlen(path);
  '$1`Index i = findSub'$1`(iRoot, path, seglen);
  if (!dot || isBad'$1`Index(i)) return i;
  return find'$1`InTree(i, dot+1);
}
'$1`Index find'$1`InTreeTop('$1`Index iRoot, const char * path) {
  char * dot = strchr(path, SEPARATOR);
  int seglen = dot ? dot-path : strlen(path);
  '$1` * pRoot = get'$1`(iRoot);
  if (seglen==strlen(nameOf'$1`(pRoot)) && 0==strncmp(path, nameOf'$1`(pRoot), seglen)) {
    if (dot)
      return find'$1`InTree(iRoot, dot+1);
    else
      return iRoot;
  }
  else
    return bad'$1`Index;
}
void append'$1`InTree('$1`Index iNew, '$1`Index iSup) {
  '$1` * pNew = get'$1`(iNew);
  append'$1`(&pNew->sups, iSup);
  if (isBad'$1`Index(iSup)) return;
  '$1` * pSup = get'$1`(iSup);
  append'$1`(&pSup->subs, iNew);
}
void prepend'$1`InTree('$1`Index iNew, '$1`Index iSup) {
  '$1` * pNew = get'$1`(iNew);
  prepend'$1`(&pNew->sups, iSup);
  if (isBad'$1`Index(iSup)) return;
  '$1` * pSup = get'$1`(iSup);
  prepend'$1`(&pSup->subs, iNew);
  prepend'$1`(&pNew->sups, iSup);
  if (isBad'$1`Index(iSup)) return;
}
')dnl

define(`PONDER_LOOK', `
PONDER_PILE('$1`To'$2`)dnl
')dnl
define(`DECL_LOOK',`
DECL_PILE('$1`To'$2`,
``  '''$1` f;
  '$2` t;
  '$1`To'$2`Index l;
  '$1`To'$2`Index g;
)

bool greater'$1`('$1`*, '$1`*); // User to define
void enter'$1`To'$2`('$1`To'$2`Index *, '$1`*, '$2`*);
bool lookup'$1`To'$2`('$1`To'$2`Index *, '$1`*, '$2`*);
')dnl
define(`IMPL_LOOK',`
IMPL_PILE('$1`To'$2`,'$3`,'$4`)
void enter_'$1`To'$2`('$1`To'$2`Index iRoot, '$1` * f, '$2` * t) {
  '$1`To'$2` * pRoot = get'$1`To'$2`(iRoot);
  if (0==memcmp(f, &pRoot->f, sizeof(pRoot->f))) {
    memcpy(&pRoot->t,t,sizeof(pRoot->t));
  } else {
    enter'$1`To'$2`(greater'$1`(f, &pRoot->f) ? &pRoot->g : &pRoot->l, f, t);
  }
}
void enter'$1`To'$2`('$1`To'$2`Index * piRoot, '$1` * f, '$2` * t) {
  if (isBad'$1`To'$2`Index(*piRoot)) {
    *piRoot = alloc'$1`To'$2`(0);
    '$1`To'$2` * p = get'$1`To'$2`(*piRoot);
    memcpy(&p->f,f,sizeof(p->f));
    memcpy(&p->t,t,sizeof(p->t));
  } else enter_'$1`To'$2`(*piRoot, f, t);
}
bool lookup'$1`To'$2`('$1`To'$2`Index * piRoot, '$1` * f, '$2` * t) {
  if (isBad'$1`To'$2`Index(*piRoot)) return false;
  '$1`To'$2` * pRoot = get'$1`To'$2`(*piRoot);
  if (0==memcmp(f, &pRoot->f, sizeof(pRoot->f))) { memcpy(t, &pRoot->t,sizeof(pRoot->t)); return true; }
  return lookup'$1`To'$2`(greater'$1`(f, &pRoot->f) ? &pRoot->g : &pRoot->l, f, t);
}
')dnl


define(`PONDER_MINHEAP',
`PONDER_PILE('$1`)
')dnl
define(`DECL_MINHEAP',
`DECL_PILE('$1`, '$2`)
bool insert'$1`('$1`, int * newLowScore); // returns if low score changed
bool extractBelow'$1`(int, '$1`*); // returns if more
')dnl

