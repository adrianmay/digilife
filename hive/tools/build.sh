#!/bin/bash

CC=clang

tools/clean.sh
rm -rf gen bin
mkdir gen bin

cp -r lit/* gen
cp -r test hive bin

usetpt() {
  TPT=$1
  XX=$2
  YY=$3
  ZZ=$4
  MOD=${XX}_${TPT}
  VAR="-v XX=${XX} -v YY=${YY} -v ZZ=${ZZ}"
  TD=`find tpt/$1 -type d`
  ID=${TD//tpt\/${TPT}/gen\/${MOD}}
  mkdir -p $ID
  X=`find ${TD} -name "*.h" -or -name "*.c" | paste -sd' '`
  declare -a TF=($X)
  IF=${TF//tpt\/${TPT}/gen\/${MOD}}
  for I in ${TF[@]}
  do
    awk ${VAR} '{ gsub(/XX/,XX); gsub(/YY/,YY); gsub(/ZZ/,ZZ); }1' $I > ${I//tpt\/${TPT}/gen\/${MOD}}
  done
}

tools/make_pile.sh Link MEGA
tools/make_meap.sh Junk MEGA
tools/make_hotel.sh Mob GIGA
tools/make_hotel.sh Msg GIGA

echo "Building tags"
find gen bin -name "*.h" -or -name "*.c" | xargs ctags || exit 1
echo "Doctoring tags"
tools/fixtags.sh  || exit 1

CS=`find gen bin -name "*.c"`
for C  in $CS
do
  O=${C/.c/.o}
  echo "Building $O"
  $CC -g -iquote gen -Wall -Werror -c $C -o $O || exit 1
done

for M in `find gen bin -maxdepth 1 -type d | grep '/'`
do
  echo "Building $M.o"
  ld --relocatable -o $M.o `find $M -name *.o` || exit 1
done

echo "Building gen/o.o"
ld --relocatable --allow-shlib-undefined -o gen/o.o gen/*.o || exit 1

echo "Building Test"
$CC -o Test gen/o.o bin/test.o || exit 1
echo "Building Hive"
$CC -o Hive gen/o.o bin/hive.o || exit 1


