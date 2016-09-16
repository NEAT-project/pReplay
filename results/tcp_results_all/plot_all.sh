#!/bin/bash

count=1;
for i in {1,6,18}; do 
	for j in {0,0.015,0.03}; do 
		for k in {0,512,2048}; do 
			bash make_plot_rtt.sh $i $j $k $count; 
			count=$((count+1));
		done;
	done;
done

pdfjam 1.pdf 2.pdf 3.pdf 4.pdf 5.pdf 6.pdf 7.pdf 8.pdf  \
	9.pdf 10.pdf 11.pdf 12.pdf 13.pdf 14.pdf 15.pdf \
	16.pdf 17.pdf 18.pdf 19.pdf 20.pdf 21.pdf 22.pdf 23.pdf \
	24.pdf 25.pdf 26.pdf 27.pdf --nup 2x2 --landscape --outfile plot_rtts.pdf
rm [0-9]*.pdf

count=1;
for i in {1,6,18}; do 
	for j in {20,80,194,800}; do 
		for k in {0,512,2048}; do 
			bash make_plot_plr.sh $i $j $k $count; 
			count=$((count+1));
		done;
	done;
done

pdfjam 1.pdf 2.pdf 3.pdf 4.pdf 5.pdf 6.pdf 7.pdf 8.pdf  \
	9.pdf 10.pdf 11.pdf 12.pdf 13.pdf 14.pdf 15.pdf \
	16.pdf 17.pdf 18.pdf 19.pdf 20.pdf 21.pdf 22.pdf 23.pdf \
	24.pdf 25.pdf 26.pdf 27.pdf 28.pdf 29.pdf 30.pdf 31.pdf \
	32.pdf 33.pdf 34.pdf 35.pdf 36.pdf --nup 2x2 --landscape --outfile plot_plrs.pdf
rm [0-9]*.pdf

count=1;
for i in {20,80,194,800}; 
	do for j in {0,0.015,0.03}; 
		do for k in {0,512,2048}; 
			do bash make_plot_cnt.sh $i $j $k $count; 
			count=$((count+1));
		done;
	done;
done

pdfjam 1.pdf 2.pdf 3.pdf 4.pdf 5.pdf 6.pdf 7.pdf 8.pdf \
	 9.pdf 10.pdf 11.pdf 12.pdf 13.pdf 14.pdf 15.pdf \
	16.pdf 17.pdf 18.pdf 19.pdf 20.pdf 21.pdf 22.pdf 23.pdf \
	24.pdf 25.pdf 26.pdf 27.pdf 28.pdf 29.pdf 30.pdf 31.pdf \
	32.pdf 33.pdf 34.pdf 35.pdf 36.pdf --nup 2x2 --landscape --outfile plot_cnt.pdf
rm [0-9]*.pdf

count=1;
for i in {1,6,18}; do 
	for j in {0,0.015,0.03}; do 
		for k in {20,80,194,800}; do 
			bash make_plot_csz.sh $i $j $k $count; 
			count=$((count+1));
		done;
	done;
done
pdfjam 1.pdf 2.pdf 3.pdf 4.pdf 5.pdf 6.pdf 7.pdf 8.pdf \
	 9.pdf 10.pdf 11.pdf 12.pdf 13.pdf 14.pdf 15.pdf \
	16.pdf 17.pdf 18.pdf 19.pdf 20.pdf 21.pdf 22.pdf 23.pdf \
	24.pdf 25.pdf 26.pdf 27.pdf 28.pdf 29.pdf 30.pdf 31.pdf \
	32.pdf 33.pdf 34.pdf 35.pdf 36.pdf --nup 2x2 --landscape --outfile plot_csz.pdf
rm [0-9]*.pdf

