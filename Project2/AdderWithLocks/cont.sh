#!/bin/bash




for i in 10000000 100000000 1000000000; do
	for j in 1 2 4 8 16 32 64 128 256; do
		m=0
		for k in 0 1 2 3 4 5 6 7 8 9; do
			b=`/home/assis/Documentos/Backup/contt $i $j`
			m=$(bc -l <<<"${b}+${m}")
		done
	m=$(bc -l <<<"${m}/10")
	echo $i,$j,$m>> out.txt
	echo $i $j
	done
done
