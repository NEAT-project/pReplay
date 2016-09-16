
site1="stackoverflow"
site2="go"
site3="theguardian"
CNT=$1
PLR=$2
CSZ=$3
plot_no=$4

egrep $site1 tmp_results_tcp | \
	awk -v cnt=$CNT -v plr=$PLR -v csz=$CSZ \
	'{if($7==cnt && $5==plr && $6==csz)print}'|\
	sort -nk4| awk '{print $4,$10}'>tmp

awk '{if($1==20){sum_1+=$2;c1+=1} else if($1==80){sum_2+=$2;c2+=1}\
	 else if($1==194){sum_3+=$2;c3+=1}\
	 else {sum_4+=$2;c4+=1}}\
	END{print "20",sum_1/c1"\n""80",sum_2/c2"\n""194",sum_3/c3"\n""800",sum_4/c4"\n"}'\
	 tmp > tmp.1 && mv tmp.1 tmp


sed '/x1 x2/d' tmp > tmp1 && mv tmp1 tmp
echo 'x1 x2' | cat - tmp > tmp1 && mv tmp1 tmp

egrep $site2 tmp_results_tcp | \
	awk -v cnt=$CNT -v plr=$PLR -v csz=$CSZ \
	'{if($7==cnt && $5==plr && $6==csz)print}'|\
	sort -nk4| awk '{print $4,$10}'>tmp.1

awk '{if($1==20){sum_1+=$2;c1+=1} else if($1==80){sum_2+=$2;c2+=1}\
	 else if($1==194){sum_3+=$2;c3+=1}\
	 else {sum_4+=$2;c4+=1}}\
	END{print "20",sum_1/c1"\n""80",sum_2/c2"\n""194",sum_3/c3"\n""800",sum_4/c4"\n"}'\
	 tmp.1 >> tmp


egrep $site3 tmp_results_tcp | \
	awk -v cnt=$CNT -v plr=$PLR -v csz=$CSZ \
	'{if($7==cnt && $5==plr && $6==csz)print}'|\
	sort -nk4| awk '{print $4,$10}'>tmp.1

awk '{if($1==20){sum_1+=$2;c1+=1} else if($1==80){sum_2+=$2;c2+=1}\
	 else if($1==194){sum_3+=$2;c3+=1}\
	 else {sum_4+=$2;c4+=1}}\
	END{print "20",sum_1/c1"\n""80",sum_2/c2"\n""194",sum_3/c3"\n""800",sum_4/c4"\n"}'\
	 tmp.1 >> tmp

Rscript plot_rtt.R $CNT $PLR $CSZ $plot_no".pdf" --save
