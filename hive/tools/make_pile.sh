echo "Making pile $1 $2"
tools/usetpt.sh pile $1 $2
echo $1_pile >> gen/${TARGET}.objs
