if [ ! -d temp_json ]; then
  mkdir -p temp_json;
fi

for file in *.har ;
	do
	fname=$file
	SFI=$IFS; IFS='_';
	set -f;
	B=($file);
	rtt=$(echo ${B[0]}|cut -d'.' -f2)
	plr=$(echo ${B[1]}|cut -d'.' -f2-)
	csz=$(echo ${B[2]}|cut -d'.' -f2)
	nc=$(echo ${B[3]}|cut -d'.' -f2)
	ptl=$(echo ${B[4]}|cut -d'.' -f2)
	count=$(echo ${B[5]}|cut -d'.' -f2)
	
	#echo $rtt $plr $csz $nc $ptl $count
	IFS=$SFI 
	set +f;
	#printf '{"RTT":%s,"PLR":%s,"CSZ":%s, "NC":%s, "PROTOCOL": "%s", "COUNT": %s}\n' "$rtt" "$plr" "$csz" \
#		"$nc" "$ptl" "$count" > temp.har
	#jq -s '.' $fname temp.har
	#jq -c '.del(.OLT)' $fname
	echo $fname
        OUT=$(mktemp temp_json/XXXX.json);
	jq -c 'del(.OLT)' $fname > temp && mv temp $OUT
	jq  --argjson rtt_ ${rtt}   '.RTT=$rtt_' $OUT > temp && mv temp $OUT
	jq  --argjson plr_ ${plr}   '.PLR=$plr_' $OUT > temp && mv temp $OUT
	jq  --argjson csz_ ${csz}   '.CSZ=$csz_' $OUT > temp && mv temp $OUT
	jq  --argjson nc_ ${nc}   '.NC=$nc_' $OUT > temp && mv temp $OUT
	jq  --arg ptl_ ${ptl}   '.PROTOCOL=$ptl_' $OUT > temp && mv temp $OUT
	jq  --argjson cnt_ ${count}   '.COUNT=$cnt_' $OUT > temp && mv temp $OUT
        cat $OUT
	#result="$rslt$(cat $file|sed 's/],//g')";
        #OUT=$(mktemp temp_json/XXXX.json);
	#echo $result > $OUT
done
