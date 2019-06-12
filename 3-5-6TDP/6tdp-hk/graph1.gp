set title "Speedup en fonction de la taille du probleme"
set ylabel "Speedup par rapport à life_seq"	    
set xlabel "Taille du probleme"
set terminal png
set output 'single.png'
plot 'res/single.dat' using 1:($2/$3) with lines title 'omp', 'res/single.dat' using 1:($2/$4) with lines title 'pthread'
