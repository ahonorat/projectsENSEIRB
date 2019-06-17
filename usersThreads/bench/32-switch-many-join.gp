set terminal png
set title "32-switch-many-join"	    
set ylabel "Temps d'execution (µs)"	    
set xlabel "Nombre de threads"
set output "32.png"
plot "32-switch-many-join.dat" using 1:(($2+$3+$4+$5+$6+$7+$8+$9+$10+$11)/10) with lines title 'Bibliothèque pthread', "32-switch-many-join.dat" using 1:(($12+$13+$14+$15+$16+$17+$18+$19+$20+$21)/10) with lines title 'Bibliothèque projet'
