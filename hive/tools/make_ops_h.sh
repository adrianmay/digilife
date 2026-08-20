echo typedef enum {
for OP in $OPS
do
  echo "  __${OP}," 
done
echo "  NUM_OPS"
echo "} Opcode;"
echo 

echo typedef enum {
for TST in $TESTS
do
  echo "  __${TST}," 
done
echo "  NUM_TESTS"
echo "} Testcode;"
echo 

echo "struct Core;"
echo "typedef enum { doingit, doneit, todoit } Doit;"

echo "typedef void Inst(struct Core *, Doit);"
for OP in $OPS
do
  echo "Inst ${OP};" 
done
echo

echo "typedef bool Test(struct Core *, Doit);"
for TST in $TESTS
do
  echo "Test ${TST};" 
done
echo

V=0
for OP in $OPS
do
  printf "#define _${OP} \"\\\\x%.2X\"\n" $V 
  V=$((V+1))
done
echo

echo "extern Inst * opfuncs[NUM_OPS];" 
echo "extern char opnames[NUM_OPS][16];" 

V=0
for TST in $TESTS
do
  printf "#define _${TST} \"\\\\x%.2X\"\n" $V 
  V=$((V+1))
done
echo

echo "extern Test * testfuncs[NUM_TESTS];" 
echo "extern char testnames[NUM_TESTS][16];" 
echo

