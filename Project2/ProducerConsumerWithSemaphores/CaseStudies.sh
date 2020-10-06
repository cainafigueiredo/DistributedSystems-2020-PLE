#!/bin/bash

`rm -f out.csv`

echo "Np,Nc,N,Time" >> out.csv

for N in 1 2 4 16 32; do
	for threads_number in 1,1 1,2 1,4 1,8 1,16 2,1 4,1 8,1 16,1; do
		IFS=',' read Np Nc <<< "${threads_number}"
		M=100000
		mean=0
		i=0
		for i in 0 1 2 3 4 5 6 7 8 9; do 
			i=$(bc -l <<< "${i}+1")
			c=`./ProducerConsumerSem -np ${Np} -nc ${Nc} -n ${N} -m ${M}`
			b=`cat elapsedTime.out`
			mean=$(bc -l <<<"${b}+${mean}")
			echo "${c}\n"
		done
	mean=$(bc -l <<<"${mean}/10")
	echo "${Np},${Nc},${N},${mean}" >> out.csv
	done
done
