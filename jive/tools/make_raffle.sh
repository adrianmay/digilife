echo "Making raffle $1 $2 $3 $4"
tools/make_hotel.sh $1 $1_raffle/Body.h $3 0
tools/usetpt.sh raffle $1 $2 $4
echo $1_raffle >> gen/${TARGET}.objs

# echo "
# #include \"${1}_raffle/structs.h\"
#
# void show${1}Body(${1}Body * p);
# " > gen/$1_hotel/$1Body.h
#
# echo "
# // #include \"${1}_raffle/${1}Ticket.h\"
# #include \"$1_hotel/$1Body.h\"
#
# void show${1}Body(${1}Body * p) {
#   show${1}Ticket(&p->ticket);
# }
# " > gen/$1_hotel/$1Body.c
