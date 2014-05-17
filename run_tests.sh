#!/bin/bash

i=10

fonc='01-main 02-switch 06-sum-tab[1..n] 07-signal-test 11-join 12-join-main 17-thread-cancel 21-create-many 22-create-many-recursive 31-switch-many 32-switch-many-join 51-fibonacci 89-preemption'

make -s lib
make -s tests
make -s tests OPT=-DUSE_PTHREAD


echo -e "###### Result comparison with pthread ######"
echo -e "(wait until all diff's are written in res_diff)\n"

echo -e "\nDiff info :"
echo -e "< thread.so"
echo -e "> p_thread\n"

echo "" > res_diff

for func in $fonc
do
./bin/$func $i $i > out
./bin/$func\_pthread $i $i > t_out
echo -e "\n=====$func==========\n" >> res_diff
diff out t_out >> res_diff
done

echo -e "Diff between our library tests results and pthread tests results (nb_thread = $i) :\n"
cat res_diff
echo -e "\nend of diff\n"


echo -e "###### Valgrind comparison with pthread ######"
echo -e "(wait until all diff's are written in val_diff)\n"

echo -e "\nDiff info :"
echo -e "< thread.so"
echo -e "> p_thread\n"

echo "" > val_diff

for func in $fonc
do
valgrind ./bin/$func $i $i 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > out
valgrind ./bin/$func\_pthread $i $i 2>&1 1>/dev/null | awk '{gsub(/^==[[:digit:]]*== /,""); gsub(/^Command:[[:print:]]*/,""); print}' > t_out
echo -e "\n=====$func==========\n" >> val_diff
diff out t_out >> val_diff 
done

echo -e "Diff between our library tests and pthread tests, using valgrind (nb_thread = $i) :\n"
cat val_diff
echo -e "\nend of diff\n"


rm out t_out
