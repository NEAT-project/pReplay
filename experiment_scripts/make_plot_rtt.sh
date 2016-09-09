#!/bin/bash

proto1="http "
proto2="phttpget "

PLR=$1
CSZ=$2
NC=$3
plot_no=$4

egrep $proto1 tmp_results | awk -v nc=$NC -v plr=$PLR -v csz=$CSZ '{if($7==nc && $5==plr&& $6==csz)print}'|sort -nk4| awk '{print $4,$10}'>tmp
awk '{if($1==20){sum_20+=$2;c20+=1} else if($1==80){sum_80+=$2;c80+=1} else{sum_194+=$2;c194+=1}}END{print "20",sum_20/c20"\n""80",sum_80/c80"\n""194",sum_194/c194"\n"}' tmp > tmp.1 && mv tmp.1 tmp
sed '/x1 x2/d' tmp > tmp1 && mv tmp1 tmp
echo 'x1 x2' | cat - tmp > tmp1 && mv tmp1 tmp

egrep $proto2 tmp_results | awk -v nc=$NC -v plr=$PLR -v csz=$CSZ '{if($7==nc && $5==plr&& $6==csz)print}'|sort -nk4| awk '{print $4,$10}'>tmp.1
awk '{if($1==20){sum_20+=$2;c20+=1} else if($1==80){sum_80+=$2;c80+=1} else{sum_194+=$2;c194+=1}}END{print "20",sum_20/c20"\n""80",sum_80/c80"\n""194",sum_194/c194"\n"}' tmp.1 >> tmp 

Rscript plot_rtt.R $NC $PLR $CSZ $plot_no".pdf" --save
