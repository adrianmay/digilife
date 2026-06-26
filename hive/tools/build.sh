#!/bin/bash

CC=gcc
CFLAGS="-O0 -pthread -g -lm -std=c23"
 
# For module M, generated stuff goes in bin/M if it exports main or gen/M if not.

# Always start with a clean slate.
tools/clean.sh
rm -rf gen bin
mkdir gen bin

# Copy simple stuff into build area
cp -r lit/* gen
cp -r test hive bin

# Stop me editing that stuff by accident
find gen bin -type f | xargs chmod -w

# Generate stuff from templates in parallel ...
pids=""
. instances.sh

# ... then wait til all done.
for p in $pids
do
  wait "$p" || exit 1
done

# I'd like the tags to point to sensible places despite the templates, and it's still not correct
echo "Building tags"
find tpt lit gen bin/test -name "*.h" -or -name "*.c" | xargs ctags || exit 1

echo "Doctoring tags"
# Point some stuff at the template instead of the generated source
awk -F'\t' -f tools/doctor.awk OFS='\t' tags > newtags
mv newtags tags
# Try to make it prefer functions to struct members when name is same
tools/sort_tags.sh

# Find sutff to compile. If ./exc exists, read a list of lines of regexes to exclude,
#   then it'll compile the rest and stop before linking.
ALL_CS=`find gen bin -name "*.c"`
# echo "ALL_CS: $ALL_CS"
if [ -f exc ]
then
  SUPPRESS="`cat exc | paste -sd '|' | grep -v '^$'`"
fi

echo "SUPPRESS: $SUPPRESS"

if [ -n "$SUPPRESS" ]
then
  CS=`echo "$ALL_CS" | grep -Pv "$SUPPRESS"`
else 
  CS=$ALL_CS
fi
echo "CS: $CS"

# Compile stuff in parallel, or if serially, it would be nice to do recently modified files first.
pids=""
for C in $CS
do
  O=${C/.c/.o}
  echo "Compiling C to $O"
  $CC $CFLAGS -iquote gen -Wall -Werror -c $C -o $O &
  pids="$pids $!"
done

# Wait on all compilations
for p in $pids
do
  wait "$p" || exit 1
done

echo "<${SUPPRESS}>"
# if [ - "$SUPPRESS" ]; then exit 3; fi
if [ -n "$SUPPRESS" ]; then exit 3; fi

# Link all generated modules to a single object, but I can't remember why:
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

# Suppress an error that occurs if a c file is all commented out:
touch gen/test.objs gen/hive.objs

# Link the exes, although it's a drag to be building hive when test runs fails
echo "Building Test"
$CC $CFLAGS -o Test bin/test.o $(cat gen/all.objs gen/test.objs | sed 's#^#gen/#; s#$#.o#') || exit 1
echo "Building Hive"
$CC $CFLAGS -o Hive bin/hive.o $(cat gen/all.objs gen/hive.objs | sed 's#^#gen/#; s#$#.o#') || exit 1

# Only need this when profiling. That will come back later for calibration.
PARA=`cat /proc/sys/kernel/perf_event_paranoid`
if [ "$PARA" -ne "1" ]
then
  sudo sysctl kernel.perf_event_paranoid=1
else
  echo "Permissions OK"
fi


