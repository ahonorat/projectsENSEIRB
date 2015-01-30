#!/bin/bash

nb_cores='1 4 16' #9 works but sizes are not compatible
sizes='256 512 1024 2048 4092'
nb_iter=50
mkdir -p res
exelist='life_seq life_omp life_pthread'
echo "Perf for : $exelist and life_mpi"
echo -e "# size \t execution time" > res/single.dat
for s in $sizes
do
    rm -f tmp
    for g in $exelist
    do
	./src/$g $nb_iter $s | grep 'time' | grep -Eo '[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?' >> tmp
    done
    var=`cat tmp | tr '\n' '\t'`
    echo -e "$s \t $var" >> res/single.dat
done

echo -e "# size \t execution time" > res/para.dat
for s in $sizes
do
    rm tmp
    for f in $nb_cores
    do
	mpiexec -np $f ./src/life_mpi $nb_iter $s | grep 'time' | grep -Eo '[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?' >> tmp
    done
    var=`cat tmp | tr '\n' '\t'`
    echo -e "$s \t $var" >> res/para.dat
done

rm tmp
