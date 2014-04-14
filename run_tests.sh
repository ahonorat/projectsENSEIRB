#!/bin/bash

i=10

make lib
make tests
make tests OPT=-DUSE_PTHREAD

echo -e "Running all tests"
./bin/01-main >> out
echo -e "01-main\n" >> t_out
./bin/01-main_pthread >> t_out
./bin/02-switch >> out
echo -e "02-switch\n" >> t_out
./bin/02-switch_pthread >> t_out
./bin/11-join >> out
echo -e "11-join\n" >> t_out
./bin/11-join_pthread >> t_out
./bin/12-join-main >> out
echo -e "12-join-main\n" >> t_out
./bin/12-join-main_pthread >> t_out
./bin/21-create-many $i >> out
echo -e "21-create-many\n" >> t_out
./bin/21-create-many_pthread $i >> t_out
./bin/22-create-many-recursive $i >> out
echo -e "22-create-many-recursive\n" >> t_out
./bin/22-create-many-recursive_pthread $i >> t_out
./bin/31-switch-many $i $i >> out
echo -e "31-switch-many\n" >> t_out
./bin/31-switch-many_pthread $i $i >> t_out
./bin/32-switch-many-join $i $i >> out
echo -e "32-switch-many-recursive\n" >> t_out
./bin/32-switch-many-join_pthread $i $i >> t_out
./bin/51-fibonacci $i >> out
echo -e "51-fibonacci\n" >> t_out
./bin/51-fibonacci_pthread $i >> t_out

echo -e "Diff between our library tests results and pthread tests results (nb_thread = $i) :\n"
diff out t_out > res_diff
cat res_diff
echo -e "\t\n"
echo -e "end of diff (should print nothing)\n"
rm out t_out
