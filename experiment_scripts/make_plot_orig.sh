
site1="stackoverflow.com_"
site2="www.amazon.co.uk_"
site3="www.theguardian.com_"
RTT=80
PLR=0.0
CSZ=0
plot_no=1
egrep $site1 rslts_from_testbed_1 | awk -v rtt=$RTT -v plr=$PLR -v csz=$CSZ '{if($4==rtt && $5==plr&& $6==csz)print}'|sort -nk7| awk '{print $7,$8}'>tmp
sed '/x1 x2/d' tmp > tmp1 && mv tmp1 tmp
echo 'x1 x2' | cat - tmp > tmp1 && mv tmp1 tmp

egrep $site2 rslts_from_testbed_1 | awk -v rtt=$RTT -v plr=$PLR -v csz=$CSZ '{if($4==rtt && $5==plr&& $6==csz)print}'|sort -nk7| awk '{print $7,$8}'>>tmp
egrep $site3 rslts_from_testbed_1 | awk -v rtt=$RTT -v plr=$PLR -v csz=$CSZ '{if($4==rtt && $5==plr&& $6==csz)print}'|sort -nk7| awk '{print $7,$8}'>>tmp

Rscript plot.R $RTT $PLR $CSZ $plot_no".pdf" --save
