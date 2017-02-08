#!/bin/bash




if [ ! -d temp_json ]; then
  mkdir -p temp_json;
fi
rm temp_json/*.json

for file in *.json ;
#for file in site.www.youtube.com.json_bwdown.2Mbit_rtt.800_plr.0_csz.0_nc.6_ptl.http_count.4.json;
	do
	fname=$file
	SFI=$IFS; IFS='_';
	set -f;
	B=($file);
	bw=$(echo ${B[1]}|cut -d'.' -f2)
	rtt=$(echo ${B[2]}|cut -d'.' -f2)
	plr=$(echo ${B[3]}|cut -d'.' -f2-)
	csz=$(echo ${B[4]}|cut -d'.' -f2)
	nc=$(echo ${B[5]}|cut -d'.' -f2)
	ptl=$(echo ${B[6]}|cut -d'.' -f2)
	count=$(echo ${B[7]}|cut -d'.' -f2)
	
#	echo $bw $rtt $plr $csz $nc $ptl $count
	IFS=$SFI 
	set +f;
        OUT=$(mktemp temp_json/XXXXX.json);
	jq -c 'del(.OLT)' $fname > temp && mv temp $OUT
	jq  --arg bw_ ${bw}   '.BW=$bw_' $OUT > temp && mv temp $OUT
	jq  --argjson rtt_ ${rtt}   '.RTT=$rtt_' $OUT > temp && mv temp $OUT
	jq  --argjson plr_ ${plr}   '.PLR=$plr_' $OUT > temp && mv temp $OUT
	jq  --argjson csz_ ${csz}   '.CSZ=$csz_' $OUT > temp && mv temp $OUT
	jq  --argjson nc_ ${nc}   '.NC=$nc_' $OUT > temp && mv temp $OUT
	jq  --arg ptl_ ${ptl}   '.PROTOCOL=$ptl_' $OUT > temp && mv temp $OUT
	jq  --argjson cnt_ ${count}   '.COUNT=$cnt_' $OUT > temp && mv temp $OUT
done

python json_reader.py > tmp_results
