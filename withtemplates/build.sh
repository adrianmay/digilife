#!/bin/bash

./clean.sh
rm -rf gen bin
mkdir gen bin

cp -r lit/* gen
cp -r exe/* bin

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

inst1=(pile Junk GIGA)
insts=(inst1)

for I in ${insts[@]}
do
  declare -n Fs=$I
  usetpt ${Fs[@]}
done

CS=`find gen bin -name "*.c"`
for C  in $CS
do
  O=${C/.c/.o}
  gcc -g -iquote gen -Wall -Werror -c $C -o $O
done

for M in `find gen bin -maxdepth 1 -type d | grep '/'`
do
  ld --relocatable -o $M.o `find $M -name *.o`
done

ld --relocatable -o gen/o.o gen/*.o

gcc -o test gen/o.o bin/test.o
gcc -o hive gen/o.o bin/hive.o
