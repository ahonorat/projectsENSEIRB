#!/bin/bash

nb_yield=10

fonc='06-sum-tab[1..n] 31-switch-many 32-switch-many-join 33-switch-many-join-LIFO' # 51-fibonacci

values_fibo='3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22'
values='10 50 100 500 1000 5000 10000'
nb_run='1 2 3 4 5 6 7 8 9 10'

make -s -C ../ lib
make -s -C ../ tests
make -s -C ../ tests OPT=-DUSE_PTHREAD

# 06 31 32 33 
for func in $fonc
do
echo -e "# $func (computing)"
echo -e "# $func (results)" > $func.dat
echo -e "# variable \t # p_thread (ms) \t # projet (ms)" > $func.dat
for i in $values
do
echo -e "$i\t" > out
for run in $nb_run
do
../bin/$func\_pthread $i $nb_yield | grep ' us' | sed 's/^.* \([0-9]*\) us$/\1/' >> out
done
echo -e "\t" >> out
for run in $nb_run
do
../bin/$func $i $nb_yield | grep ' us' | sed 's/^.* \([0-9]*\) us$/\1/' >> out
done
tr '\n' ' ' < out > tmp
echo " " >> tmp
cat tmp >> $func.dat
done
#gnuplot
gnuplot <<EOF
load '$func.gp'
EOF
echo -e "--->Graphic generated"
done


# 51
func=51-fibonacci

echo -e "# $func (computing)"
echo -e "# $func (results)" > $func.dat
echo -e "# variable \t # p_thread (ms) \t # projet (ms)" > $func.dat
for i in $values_fibo
do
echo -e "$i\t" > out
for run in $nb_run
do
../bin/$func\_pthread $i | grep ' us' | sed 's/^.* \([0-9]*\) us$/\1/' >> out
done
echo -e "\t" >> out
for run in $nb_run
do
../bin/$func $i | grep ' us' | sed 's/^.* \([0-9]*\) us$/\1/' >> out
done
tr '\n' ' ' < out > tmp
echo " " >> tmp
cat tmp >> $func.dat
done
#gnuplot
gnuplot <<EOF
load '$func.gp'
EOF
echo -e "--->Graphic generated"

rm -f out
rm -f tmp
