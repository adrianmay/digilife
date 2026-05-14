#!/bin/bash

CC=gcc

tools/clean.sh
rm -rf gen bin
mkdir gen bin

cp -r lit/* gen
cp -r test hive bin
find gen bin -type f | xargs chmod -w

export TARGET=test
tools/make_pile.sh   Link  bit/Link.h       MEGA   || exit 1
tools/make_meap.sh   Junk  bit/Junk.h       MEGA   || exit 1
tools/make_hotel.sh  Thing bit/ThingBody.h  GIGA 0 || exit 1
tools/make_raffle.sh Mess  bit/MessTicket.h GIGA   || exit 1

export TARGET=hive
# tools/make_hotel.sh Mob GIGA 1
# tools/make_raffle.sh Msg GIGA

echo "Building tags"
find gen bin -name "*.h" -or -name "*.c" | xargs ctags || exit 1

echo "Doctoring tags"
awk -F'\t' -f tools/doctor.awk OFS='\t' tags > newtags
mv newtags tags

CS=`find gen bin -name "*.c"`
for C  in $CS
do
  O=${C/.c/.o}
  echo "Compiling C to $O"
  $CC -pthread -g -iquote gen -Wall -Werror -c $C -o $O || exit 1
done

for M in `find gen bin -maxdepth 1 -type d | grep '/'`
do
  OS=`find $M -name *.o` 
  if [[ -n $OS ]]
  then
    echo "Building module object $M.o"
    ld --relocatable -o $M.o $OS || exit 1
  fi
done

# X=(gen/*.o)
# echo "Main objects:  ${X[@]}"
# ld --relocatable --allow-shlib-undefined -o gen/o.o gen/*.o || exit 1

echo "Building Test"
$CC -pthread -o Test bin/test.o $(cat gen/all.objs gen/test.objs | sed 's#^#gen/#; s#$#.o#') || exit 1
echo "Building Hive"
$CC -pthread -o Hive bin/hive.o $(cat gen/all.objs gen/hive.objs | sed 's#^#gen/#; s#$#.o#') || exit 1

PARA=`cat /proc/sys/kernel/perf_event_paranoid`
if [ "$PARA" -ne "1" ]
then
  sudo sysctl kernel.perf_event_paranoid=1
else 
  echo "Permissions OK"
fi


