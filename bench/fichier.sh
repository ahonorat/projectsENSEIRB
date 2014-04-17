#!/bin/bash

cd ..
for i in 1 2 3 4 5 6 7 8 9 10
do
echo $i
for j in 1 2 3 4 5 6 7 8 9 10
do
./bin/32-switch-many-join $i*10 10
./bin/32-switch-many-join_pthread $k*10 10
done
done
exit 0
