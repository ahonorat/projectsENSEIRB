#!/bin/bash

rm -f res/*.png
cd res
gnuplot <<EOF
set title "Temps d'execution en fonction de la taille du probleme"
set ylabel "Temps d'execution (µs)"	    
set xlabel "Taille du probleme"
set terminal png
set output 'single.png'
plot 'single.dat' using 1:2 with lines title 'seq', 'single.dat' using 1:3 with lines title 'omp', 'single.dat' using 1:4 with lines title 'pthread'
EOF

gnuplot <<EOF
set title "Temps d'execution en fonction de la taille du probleme"
set ylabel "Temps d'execution (µs)"	    
set xlabel "Taille du probleme"
set terminal png
set output 'para.png'
plot 'para.dat' using 1:2 with lines title '1 coeur', 'para.dat' using 1:3 with lines title '4 coeurs', 'para.dat' using 1:4 with lines title '16 coeurs'
EOF
