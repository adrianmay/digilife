echo "#include \"ops.h\""
echo

echo "Inst * opfuncs[NUM_OPS] = {" 
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

echo "Test * testfuncs[NUM_TESTS] = {" 
for TST in $TESTS
do
  echo "  ${TST}," 
done
echo "};"
echo

echo "char testnames[NUM_TESTS][16] = {" 
for TST in $TESTS
do
  echo "  \"${TST}\"," 
done
echo "};"
echo
