#!/bin/bash

i=10

make -s lib
make -s tests
make -s tests OPT=-DUSE_PTHREAD


echo -e "\nDiff info :"
echo -e "< thread.so"
echo -e "> p_thread\n"

echo -e "Running all tests in 3 secs"
sleep 1;
echo -e "Running all tests in 2 secs"
sleep 1;
echo -e "Running all tests in 1 secs"
sleep 1;
echo -e "Running all tests in 0 secs\n"



echo -e "###### Result comparison with pthread ######"
echo -e "(wait until all diff's are written in res_diff)\n"

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
echo -e "\nend of diff\n"


echo -e "###### Valgrind comparison with pthread ######"
echo -e "(wait until all diff's are written in val_diff)\n"

valgrind ./bin/01-main 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > out
valgrind ./bin/01-main_pthread 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > t_out
echo -e "\n=====01-main==========\n" > val_diff
diff out t_out >> val_diff 

valgrind ./bin/02-switch 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > out
valgrind ./bin/02-switch_pthread 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > t_out
echo -e "\n=====02-switch=====\n" >> val_diff
diff out t_out >> val_diff

valgrind ./bin/11-join 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > out
valgrind ./bin/11-join_pthread 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > t_out
echo -e "\n=====11-join=====\n" >> val_diff
diff out t_out >> val_diff

valgrind ./bin/12-join-main 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > out
valgrind ./bin/12-join-main_pthread 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > t_out
echo -e "\n=====12-join-main=====\n" >> val_diff
diff out t_out >> val_diff

valgrind ./bin/21-create-many $i 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > out
valgrind ./bin/21-create-many_pthread $i 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > t_out
echo -e "\n=====21-create-many=====\n" >> val_diff
diff out t_out >> val_diff

valgrind ./bin/22-create-many-recursive $i 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > out
valgrind ./bin/22-create-many-recursive_pthread $i 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > t_out
echo -e "\n=====22-create-many-recursive=====\n" >> val_diff
diff out t_out >> val_diff

valgrind ./bin/31-switch-many $i $i 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > out
valgrind ./bin/31-switch-many_pthread $i $i 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > t_out
echo -e "\n=====31-switch-many=====\n" >> val_diff
diff out t_out >> val_diff

valgrind ./bin/32-switch-many-join $i $i 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > out
valgrind ./bin/32-switch-many-join_pthread $i $i 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > t_out
echo -e "\n=====32-switch-many-recursive=====\n" >> val_diff
diff out t_out >> val_diff

valgrind ./bin/51-fibonacci $i 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > out
valgrind ./bin/51-fibonacci_pthread $i 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > t_out
echo -e "\n=====51-fibonacci=====\n" >> val_diff
diff out t_out >> val_diff

echo -e "Diff between our library tests and pthread tests, using valgrind (nb_thread = $i) :\n"
cat val_diff
echo -e "\nend of diff\n"


rm out t_out
