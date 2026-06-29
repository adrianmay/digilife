echo "Making hotel $1 $2 $3 $4"
tools/make_pile.sh "$1" $1_hotel/record.h $3 1
tools/make_meap.sh "$1Bomb" $1_hotel/bomb.h $3
tools/usetpt.sh hotel $1 $2 $4
echo $1_hotel >> gen/${TARGET}.objs

