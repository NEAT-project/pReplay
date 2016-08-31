#!/bin/sh

a=0
while [ "$a" -lt 10 ]    # this is loop1
do
	./pReplay bsd3.nplab.de tree_folder/www.theguardian.com_us.json
	a=`expr $a + 1`
done
