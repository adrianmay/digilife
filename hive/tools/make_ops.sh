Insts="nop end iff elsif prs prf" # roll spawn post"
Tests="no yes like gt not"
Floats="zero one two imm cmob cmsg rndl rndl_ rndg add mul inv neg"
Rcvrs="me sndr child rndrc rc0 rc1 rc2 rc3"


function make_h_tables() {
  V="${1}s"
  echo typedef enum {
  for X in ${!V}; do echo "  __${X},"; done
  echo "  Num${1}s"; echo "} $1Code;"; echo 
  N=0
  for X in ${!V}; do printf "#define ${X^^} \"\\\\x%.2X\"\n" $N; N=$((N+1)); done
  echo
}

function make_h() {
  echo
  make_h_tables Inst void
  make_h_tables Test bool
  make_h_tables Float float
  make_h_tables Peer MobTact 
}

function make_c_tables() {
  echo "typedef $2 $1(Core *, Doit);"
  echo "extern $1 * funcsFor${1}s[Num${1}s];"
  echo "extern char namesOf${1}s[Num${1}s][16];"
  echo
  V="${1}s"
  for X in ${!V}; do echo "$1 ${X};"; done
  echo
  echo "$1 * funcsFor${1}s[Num${1}s] = {"
  for X in ${!V}; do echo "  ${X},"; done
  echo "};"; echo
  echo "char namesOf${1}s[Num${1}s][16] = {"
  for X in ${!V}; do echo "  \"${X}\","; done
  echo "};"; echo
}

function make_c() {
  echo
  make_c_tables Inst void
  make_c_tables Test bool
  make_c_tables Float float
  make_c_tables Peer MobTact 
  echo
}

make_h > gen/core/ops.h
make_c > gen/core/ops.cc
