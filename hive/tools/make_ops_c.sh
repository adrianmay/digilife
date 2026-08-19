echo "#include \"ops.h\""
echo
echo "Instruction * opfuncs[NUM_OPS] = {" 
for OP in $OPS
do
  echo "  ${OP}," 
done
echo "};"
echo
echo "char opnames[NUM_OPS][16] = {" 
for OP in $OPS
do
  echo "  \"${OP}\"," 
done
echo "};"
echo
