#!/bin/bash

for i in {'stackoverflow','go','theguardian'}; do
	for j in {0,0.015,0.03}; do
		for k in {0,512,2048}; do
			python json_reader.py jsons/ $i $j $k
		done
	done
done
		
