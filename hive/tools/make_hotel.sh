echo "Making hotel $1 $2"
tools/make_pile.sh "$1Bulk" $2
tools/make_meap.sh "$1Bomb" $2
tools/usetpt.sh hotel $1

