#!/bin/bash

CC=gcc

tools/clean.sh
rm -rf gen bin
mkdir gen bin

cp -r lit/* gen
cp -r test hive bin
find gen bin -type f | xargs chmod -w

tools/make_pile.sh Link MEGA || exit 1
tools/make_meap.sh Junk MEGA tocks || exit 1
tools/make_hotel.sh Thing GIGA
tools/make_hotel.sh Mob GIGA
tools/make_raffle.sh Msg GIGA

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
  echo "Might build module object $M.o"
  OS=`find $M -name *.o` 
  if [[ -n $OS ]]
  then
    echo "Building module object $M.o"
    ld --relocatable -o $M.o $OS || exit 1
  fi
done

X=(gen/*.o)
echo "Main objects:  ${X[@]}"
# ld --relocatable --allow-shlib-undefined -o gen/o.o gen/*.o || exit 1

echo "Building Test"
$CC -pthread -o Test bin/test.o gen/Junk_meap.o gen/Junk_pile.o gen/Link_pile.o gen/ThingBomb_meap.o gen/ThingBomb_pile.o gen/ThingBulk_pile.o gen/Thing_hotel.o gen/globals.o gen/ipile.o gen/misc.o gen/perf.o || exit 1
echo "Building Hive"
$CC -pthread -o Hive bin/hive.o gen/MobBomb_meap.o gen/MobBomb_pile.o gen/MobBulk_pile.o gen/Mob_hotel.o gen/globals.o gen/ipile.o gen/misc.o gen/perf.o || exit 1

PARA=`cat /proc/sys/kernel/perf_event_paranoid`
if [ "$PARA" -ne "1" ]
then
  sudo sysctl kernel.perf_event_paranoid=1
else 
  echo "Permissions OK"
fi


