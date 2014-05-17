set terminal png
set title "51-fibonacci"	    
set ylabel "Temps d'execution (ms)"	    
set xlabel "Entier en entrée"
set output "51.png"
plot "51-fibonacci.dat" using 1:(($2+$3+$4+$5+$6+$7+$8+$9+$10+$11)/10) with lines title 'Bibliothèque pthread', "51-fibonacci.dat" using 1:(($12+$13+$14+$15+$16+$17+$18+$19+$20+$21)/10) with lines title 'Bibliothèque projet'
