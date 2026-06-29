echo "Making meap $1 $2 $3"
tools/make_pile.sh $1 $2 $3 0
tools/usetpt.sh meap $1 $2 $3
echo $1_meap >> gen/${TARGET}.objs
