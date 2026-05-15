
chmod +w gen/all.objs
export TARGET=all
tools/make_hotel.sh  Mob   bit/MobBody.h    GIGA 1 
pids="$pids $!"
tools/make_raffle.sh Msg   bit/MsgTicket.h  GIGA Mob  
pids="$pids $!"

export TARGET=test
tools/make_pile.sh   Link  bit/Link.h       MEGA    
pids="$pids $!"
tools/make_meap.sh   Junk  bit/Junk.h       MEGA   
pids="$pids $!"
tools/make_hotel.sh  Thing bit/ThingBody.h  GIGA 1 
pids="$pids $!"
tools/make_raffle.sh Mess  bit/MessTicket.h GIGA Thing  
pids="$pids $!"

