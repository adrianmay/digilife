#!/bin/bash

CC=gcc
CFLAGS="-O0 -pthread -g -lm -std=c23"

tools/clean.sh
rm -rf gen bin
mkdir gen bin

cp -r lit/* gen
cp -r test hive bin
find gen bin -type f | xargs chmod -w

pids=""
. instances.sh

for p in $pids
do
  wait "$p" || exit 1
done

echo "Building tags"
find tpt lit gen bin/test -name "*.h" -or -name "*.c" | xargs ctags || exit 1

echo "Doctoring tags"
awk -F'\t' -f tools/doctor.awk OFS='\t' tags > newtags
mv newtags tags
tools/sort_tags.sh

pids=""

CS=`find gen bin -name "*.c"`
for C  in $CS
do
  O=${C/.c/.o}
  echo "Compiling C to $O"
  $CC $CFLAGS -iquote gen -Wall -Werror -c $C -o $O &
  pids="$pids $!"
done

for p in $pids
do
  wait "$p" || exit 1
done

pids=""

for M in `find gen bin -maxdepth 1 -type d | grep '/'`
do
  OS=`find $M -name *.o`
  if [[ -n $OS ]]
  then
    echo "Building module object $M.o"
    ld --relocatable -o $M.o $OS &
    pids="$pids $!"
  fi
done

for p in $pids
do
  wait "$p" || exit 1
done

# X=(gen/*.o)
# echo "Main objects:  ${X[@]}"
# ld --relocatable --allow-shlib-undefined -o gen/o.o gen/*.o || exit 1
touch gen/test.objs gen/hive.objs
echo "Building Test"
$CC $CFLAGS -o Test bin/test.o $(cat gen/all.objs gen/test.objs | sed 's#^#gen/#; s#$#.o#') || exit 1
echo "Building Hive"
$CC $CFLAGS -o Hive bin/hive.o $(cat gen/all.objs gen/hive.objs | sed 's#^#gen/#; s#$#.o#') || exit 1

PARA=`cat /proc/sys/kernel/perf_event_paranoid`
if [ "$PARA" -ne "1" ]
then
  sudo sysctl kernel.perf_event_paranoid=1
else
  echo "Permissions OK"
fi


