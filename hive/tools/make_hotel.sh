echo "Making hotel $1 $2 $3"
tools/make_pile.sh "$1Bulk" $1_hotel/Bulk.h $3
tools/make_meap.sh "$1Bomb" $1_hotel/Bomb.h $3
tools/usetpt.sh hotel $1 $2
echo $1_hotel >> gen/${TARGET}.objs

# # Tell the piles to get their struct from the hotel.
# echo "
# #include \"${1}_hotel/Bomb.h\"
# " > gen/$1Bomb_pile/$1Bomb.h
# 
# echo "
# #include \"${1}_hotel/Bulk.h\"
# " > gen/$1Bulk_pile/$1Bulk.h

