echo "Making hotel $1 $2"
tools/make_pile.sh "$1Bulk" $2 1
tools/make_meap.sh "$1Bomb" $2
tools/usetpt.sh hotel $1

echo "
#include \"${1}Bulk_pile/1.h\"
#include \"${1}_hotel/Bomb.h\"
" > gen/$1Bomb_pile/$1Bomb.h

echo "
#include \"${1}Bomb_pile/1.h\"
#include \"${1}_hotel/Bulk.h\"
" > gen/$1Bulk_pile/$1Bulk.h

