#!/bin/bash

count=1;for i in {20,80,194}; do for j in {0,0.03}; do for k in {0,512,2048}; do bash make_plot_cnt.sh $i $j $k $count; count=$((count+1));done;done;done
pdfjam 1.pdf 2.pdf 3.pdf 4.pdf 5.pdf 6.pdf 7.pdf 8.pdf 9.pdf 10.pdf 11.pdf 12.pdf --nup 3x4 --landscape --outfile Page1.pdf
pdfdfjam 13.pdf 14.pdf 15.pdf 16.pdf 17.pdf 18.pdf   --nup 3x4 --landscape --outfile Page2.pdf
mkdir based_on_cnts
mv [0-9]*.pdf based_on_cnts/

count=1;for i in {1,6,18}; do for j in {0,0.03}; do for k in {0,512,2048}; do bash make_plot_rtt.sh $i $j $k $count; count=$((count+1));done;done;done
pdfjam 1.pdf 2.pdf 3.pdf 4.pdf 5.pdf 6.pdf 7.pdf 8.pdf 9.pdf 10.pdf 11.pdf 12.pdf --nup 3x4 --landscape --outfile Page3.pdf
pdfdfjam 13.pdf 14.pdf 15.pdf 16.pdf 17.pdf 18.pdf   --nup 3x4 --landscape --outfile Page4.pdf
mkdir based_on_rtts
mv [0-9]*.pdf based_on_rtts/

count=1;for i in {1,6,18}; do for j in {20,80,194}; do for k in {0,512,2048}; do bash make_plot_plr.sh $i $j $k $count; count=$((count+1));done;done;done
pdfjam 1.pdf 2.pdf 3.pdf 4.pdf 5.pdf 6.pdf 7.pdf 8.pdf 9.pdf 10.pdf 11.pdf 12.pdf --nup 3x4 --landscape --outfile Page5.pdf
pdfdfjam 13.pdf 14.pdf 15.pdf 16.pdf 17.pdf 18.pdf   --nup 3x4 --landscape --outfile Page6.pdf
mkdir based_on_plrs
mv [0-9]*.pdf based_on_plrs/

count=1;for i in {1,6,18}; do for j in {0,0.03}; do for k in {20,80,194}; do bash make_plot_csz.sh $i $j $k $count; count=$((count+1));done;done;done
pdfjam 1.pdf 2.pdf 3.pdf 4.pdf 5.pdf 6.pdf 7.pdf 8.pdf 9.pdf 10.pdf 11.pdf 12.pdf --nup 3x4 --landscape --outfile Page7.pdf
pdfdfjam 13.pdf 14.pdf 15.pdf 16.pdf 17.pdf 18.pdf   --nup 3x4 --landscape --outfile Page8.pdf
mkdir based_on_csz
mv [0-9]*.pdf based_on_csz/


