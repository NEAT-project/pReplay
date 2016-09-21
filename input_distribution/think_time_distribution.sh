#!/bin/bash

DIR_NAME=$1

egrep -h "\"time\""  $DIR_NAME/*.json |\
	egrep -v "s_time|e_time|-1"|\
	 sed 's/[^0-9.]//g'
#	| awk '{if ($1==0) {print 0.000001}else{print $1}}' 




