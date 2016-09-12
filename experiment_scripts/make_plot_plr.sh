#!/bin/bash

proto1="http "
proto2="phttpget "

RTT=$1
CSZ=$2
NC=$3
plot_no=$4

egrep $proto1 tmp_results | \
	awk -v nc=$NC -v rtt=$RTT -v csz=$CSZ \
	'{if($7==nc && $4==rtt && $6==csz)print}'|\
	sort -nk5| awk '{print $5,$10}'>tmp

awk '{if($1==0){sum_0+=$2;c0+=1}  else{sum_1+=$2;c1+=1}}\
	END{print "0",sum_0/c0"\n""0.03",sum_1/c1"\n"}' \
	tmp > tmp.1 && mv tmp.1 tmp
sed '/x1 x2/d' tmp > tmp1 && mv tmp1 tmp
echo 'x1 x2' | cat - tmp > tmp1 && mv tmp1 tmp

egrep $proto2 tmp_results | \
	awk -v nc=$NC -v rtt=$RTT -v csz=$CSZ \
	'{if($7==nc && $4==rtt && $6==csz)print}'|\
	sort -nk5| awk '{print $5,$10}'>tmp.1

awk '{if($1==0){sum_0+=$2;c0+=1}  else{sum_1+=$2;c1+=1}}\
	END{print "0",sum_0/c0"\n""0.03",sum_1/c1"\n"}' \
	tmp.1 >> tmp

Rscript plot_plr.R $NC $RTT $CSZ $plot_no".pdf" --save
