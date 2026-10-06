# Print instructions bill themselves by length
   Insts="nop end iff elsif post spawn prs prf prp disas"
 InstCpu="  1   1   3     3  200  1000   0   9  20     0"
   Tests="no yes not like gt"
 TestCpu=" 1   1   1   10  3"
  Floats="zero one two imm csh cyc rndl rndg add mul inv neg"
FloatCpu="   1   1   1   3   1   1   20   50   5  10  15   3"
   Peers="me sndr child peer0 peer1 peer2 peer3"
 PeerCpu=" 1    1     1     1     1     1     1"

Values="10 100 1000 10000 100000 1000000 2 3 4 5 6 7 8 9" 

PROTOS="typedef enum { " 
function make_h_tables() {
  PROTOS="${PROTOS}${1}Proto, "
  V="${1}s"
  echo typedef enum {
  for X in ${!V}; do echo "  __${X},"; done
  echo "  Num${1}s"; echo "} $1Code;"; echo 
  N=0
  for X in ${!V}; do printf "#define ${X^^} \"\\\\x%.2X\"\n" $N; N=$((N+1)); done
  echo
}

function make_h() {
  echo "#define MAX_IN_PROTO 20"
  make_h_tables Inst void
  make_h_tables Test bool
  make_h_tables Float float
  make_h_tables Peer MobTact 
  echo "$PROTOS NumProtos } Proto;"
  echo
  for V in $Values; do
    B=`python3 -c 'import struct,sys; print("".join(f"\\\\x{x:02X}" for x in struct.pack("<f", float(sys.argv[1]))))' ${V}`
    echo "#define  V_${V} \"$B\""
  done
}

function make_c_tables() {
  cat <<HERE
typedef $2 $1(Core *, Mode *);
$1 do$1;
extern $1 * funcsFor${1}s[Num${1}s];
extern char namesOf${1}s[Num${1}s][16];

HERE
#  echo "typedef void $1Quiner(Core *);"
#  echo "void quine${1}(uint8_t, Core *);"
#  echo "extern $1Quiner * quinersFor${1}s[Num${1}s];"
  V="${1}s"
  for X in ${!V}; do echo "$1 ${X};"; done
  echo
#  for X in ${!V}; do echo "$1Quiner ${X}Q;"; done
#  echo
  echo "$1 * funcsFor${1}s[Num${1}s] = {"
  for X in ${!V}; do echo "  ${X},"; done
  echo "};" 
  echo
#  echo "$1Quiner * quinersFor${1}s[Num${1}s] = {"
#  for X in ${!V}; do echo "  ${X}Q,"; done
#  echo "};" 
  echo
  echo "char namesOf${1}s[Num${1}s][16] = {"
  for X in ${!V}; do echo "  \"${X^^}\","; done
  echo "};"
  echo
  V="${1}Cpu"
  echo "int cpuCyclesOf${1}s[Num${1}s] = {"
  for X in ${!V}; do echo "  ${X},"; done
  echo "};"
  echo
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

