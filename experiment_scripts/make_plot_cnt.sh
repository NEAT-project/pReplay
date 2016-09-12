#!/bin/bash

proto1="http "
proto2="phttpget "

RTT=$1
CSZ=$2
PLR=$3
plot_no=$4

egrep $proto1 tmp_results | \
	awk -v rtt=$RTT -v plr=$PLR -v csz=$CSZ \
	'{if($4==rtt && $5==plr && $6==csz)print}'|\
	sort -nk7| awk '{print $7,$10}'>tmp

awk '{if($1==1){sum_1+=$2;c1+=1} else if($1==6){sum_6+=$2;c6+=1}\
	 else{sum_18+=$2;c18+=1}}\
	END{print "1",sum_1/c1"\n""6",sum_6/c6"\n""18",sum_18/c18"\n"}'\
	 tmp > tmp.1 && mv tmp.1 tmp

sed '/x1 x2/d' tmp > tmp1 && mv tmp1 tmp
echo 'x1 x2' | cat - tmp > tmp1 && mv tmp1 tmp

egrep $proto2 tmp_results | \
	awk -v rtt=$RTT -v plr=$PLR -v csz=$CSZ \
	'{if($4==rtt && $5==plr && $6==csz)print}'|\
	sort -nk7| awk '{print $7,$10}'>tmp.1

awk '{if($1==1){sum_1+=$2;c1+=1} else if($1==6){sum_6+=$2;c6+=1}\
	 else{sum_18+=$2;c18+=1}}\
	END{print "1",sum_1/c1"\n""6",sum_6/c6"\n""18",sum_18/c18"\n"}'\
	 tmp.1 >> tmp


Rscript plot_cnt.R $RTT $PLR $CSZ $plot_no".pdf" --save
