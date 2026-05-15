export TARGET=test
tools/make_pile.sh   Link  bit/Link.h       MEGA   || exit 1
tools/make_meap.sh   Junk  bit/Junk.h       MEGA   || exit 1
tools/make_hotel.sh  Thing bit/ThingBody.h  GIGA 1 || exit 1
tools/make_raffle.sh Mess  bit/MessTicket.h GIGA   || exit 1

export TARGET=hive
## tools/make_hotel.sh Mob GIGA 1
## tools/make_raffle.sh Msg GIGA
