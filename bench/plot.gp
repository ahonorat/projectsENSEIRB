#31
#plot "31.dat" with lines, "31pthread.dat" with lines

#32
#plot "32.dat" using 1:2 with lines, "32.dat" using 1:3 with lines
set title "Exemple d'utilisation de gnuplot"	    
set xlabel "Temps d'execution (ms)"	    
set ylabel "Nb threads"
plot "exemple.dat" using 1:(($2+$3+$4+$5+$6+$7+$8+$9+$10+$11)/10) with lines title 'temps pthread', "exemple.dat" using 1:(($12+$13+$14+$15+$16+$17+$18+$19+$20+$21)/10) with lines title 'notre temps'
pause -1

