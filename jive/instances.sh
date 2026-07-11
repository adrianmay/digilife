chmod +w gen/all.objs
export TARGET=all

export TARGET=test
tools/make_pile.sh   Link  bit/Link.h   MEGA 1 &
pids="$pids $!"
tools/make_meap.sh   Junk  bit/Junk.h   MEGA &
pids="$pids $!"
tools/make_hotel.sh  Thing bit/Thing.h  GIGA 1 &
pids="$pids $!"
tools/make_raffle.sh Mess  bit/Mess.h   GIGA &
pids="$pids $!"


