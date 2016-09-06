#!/bin/sh

a=0
while [ "$a" -lt 10 ]    # this is loop1
do
	./pReplay localhost tree_folder/www.theguardian.com_us.json phttpget
	a=`expr $a + 1`
	sleep 5
done

echo "TCP part"

a=0
while [ "$a" -lt 10 ]    # this is loop1
do
   	./pReplay localhost tree_folder/www.theguardian.com_us.json
        a=`expr $a + 1`
        sleep 5
done
