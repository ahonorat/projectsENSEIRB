#!/bin/bash

rm -f bin/*.png
echo "png will be generated for:"
filelist=`echo $(ls bin/data_dgetrf*)`
echo $filelist
for f in $filelist
do
mat_size=`grep 'square matrix' $f | grep -o [0-9]*`
gnuplot <<EOF
set title "Temps d'execution en fonction de la taille des blocs"
set ylabel "Temps d'execution (ms)"	    
set xlabel "Taille des blocs carrés"
set terminal png
set output '$f.png'
plot '$f' using 1:2 with lines title 'MAT_SIZE = $mat_size'
EOF
done

