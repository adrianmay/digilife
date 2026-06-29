echo "Making pile $1 $2 $3 $4"
tools/usetpt.sh pile $1 $2 $3 $4
echo $1_pile >> gen/${TARGET}.objs
