
site1="stackoverflow"
site2="go"
site3="theguardian"
CNT=$1
RTT=$2
CSZ=$3
plot_no=$4
egrep $site1 tmp_results_tcp | \
	awk -v rtt=$RTT -v csz=$CSZ -v cnt=$CNT \
	'{if($4==rtt && $6==csz && $7==cnt)print}'|\
	sort -nk5| awk '{print $5,$10}'>tmp

awk '{if($1==0){sum_1+=$2;c1+=1} else if($1==0.015){sum_6+=$2;c6+=1}\
	 else{sum_18+=$2;c18+=1}}\
	END{print "0",sum_1/c1"\n""1.5",sum_6/c6"\n""3",sum_18/c18"\n"}'\
	 tmp > tmp.1 && mv tmp.1 tmp


sed '/x1 x2/d' tmp > tmp1 && mv tmp1 tmp
echo 'x1 x2' | cat - tmp > tmp1 && mv tmp1 tmp

egrep $site2 tmp_results_tcp | \
	awk -v rtt=$RTT -v csz=$CSZ -v cnt=$CNT \
	'{if($4==rtt && $6==csz && $7==cnt)print}'|\
	sort -nk5| awk '{print $5,$10}'>tmp.1

awk '{if($1==0){sum_1+=$2;c1+=1} else if($1==0.015){sum_6+=$2;c6+=1}\
	 else{sum_18+=$2;c18+=1}}\
	END{print "0",sum_1/c1"\n""1.5",sum_6/c6"\n""3",sum_18/c18"\n"}'\
	 tmp.1 >> tmp

egrep $site3 tmp_results_tcp | \
	awk -v rtt=$RTT -v csz=$CSZ -v cnt=$CNT \
	'{if($4==rtt && $6==csz && $7==cnt)print}'|\
	sort -nk5| awk '{print $5,$10}'>tmp.1

awk '{if($1==0){sum_1+=$2;c1+=1} else if($1==0.015){sum_6+=$2;c6+=1}\
	 else{sum_18+=$2;c18+=1}}\
	END{print "0",sum_1/c1"\n""1.5",sum_6/c6"\n""3",sum_18/c18"\n"}'\
	 tmp.1 >> tmp

Rscript plot_plr.R $CNT $RTT $CSZ $plot_no".pdf" --save
