echo "Making raffle $1 $2"
tools/make_hotel.sh $1 $2
tools/usetpt.sh raffle $1

echo "
#include \"${1}_raffle/structs.h\"
" > gen/$1_hotel/$1Body.h
