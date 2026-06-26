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
  O=${I//tpt\/${TPT}/gen\/${MOD}}
  awk ${VAR} '{ gsub(/XX/,XX); gsub(/YY/,YY); gsub(/ZZ/,ZZ); }1' $I > $O
  chmod -w $O
done

