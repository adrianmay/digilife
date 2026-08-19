echo typedef enum {
for OP in $OPS
do
  echo "  __${OP}," 
done
echo "  NUM_OPS"
echo "} Opcode;"
echo 
echo "struct Core;"
echo "typedef int Instruction(struct Core *, bool);"
echo 
for OP in $OPS
do
  echo "Instruction ${OP};" 
done
echo

V=0
for OP in $OPS
do
  printf "#define _${OP} \"\\\\x%.2X\"\n" $V 
  V=$((V+1))
done
echo
echo "extern Instruction * opfuncs[NUM_OPS];" 
echo "extern char opnames[NUM_OPS][16];" 
echo
