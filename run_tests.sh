#!/bin/bash

i=10

make lib
make tests
make tests OPT=-DUSE_PTHREAD

echo -e "\nRunning all tests\n"

./bin/01-main > out
./bin/01-main_pthread > t_out
echo -e "\n=====01-main==========\n" > res_diff
diff out t_out >> res_diff

./bin/02-switch > out
./bin/02-switch_pthread > t_out
echo -e "\n=====02-switch=====\n" >> res_diff
diff out t_out >> res_diff

./bin/11-join > out
./bin/11-join_pthread > t_out
echo -e "\n=====11-join=====\n" >> res_diff
diff out t_out >> res_diff

./bin/12-join-main > out
./bin/12-join-main_pthread > t_out
echo -e "\n=====12-join-main=====\n" >> res_diff
diff out t_out >> res_diff

./bin/21-create-many $i > out
./bin/21-create-many_pthread $i > t_out
echo -e "\n=====21-create-many=====\n" >> res_diff
diff out t_out >> res_diff

./bin/22-create-many-recursive $i > out
./bin/22-create-many-recursive_pthread $i > t_out
echo -e "\n=====22-create-many-recursive=====\n" >> res_diff
diff out t_out >> res_diff

./bin/31-switch-many $i $i > out
./bin/31-switch-many_pthread $i $i > t_out
echo -e "\n=====31-switch-many=====\n" >> res_diff
diff out t_out >> res_diff

./bin/32-switch-many-join $i $i > out
./bin/32-switch-many-join_pthread $i $i > t_out
echo -e "\n=====32-switch-many-recursive=====\n" >> res_diff
diff out t_out >> res_diff

./bin/51-fibonacci $i > out
./bin/51-fibonacci_pthread $i > t_out
echo -e "\n=====51-fibonacci=====\n" >> res_diff
diff out t_out >> res_diff

echo -e "Diff between our library tests results and pthread tests results (nb_thread = $i) :\n"
cat res_diff
echo -e "\n"
echo -e "end of diff\n"
rm out t_out
