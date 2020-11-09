#!/bin/bash

r=100
k=1
n=2

saida_arquivo="resultados_n_${n}_r_${r}_k_${k}"

for i in $(seq 1 ${n}); do
	b=`./process ${r} ${k} ${saida_arquivo}` &
	#sleep 1
done
