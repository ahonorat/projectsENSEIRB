set terminal png
set title "Exemple de somme d'un tableau par méthode diviser pour régner"	    
set ylabel "Temps d'execution (ms)"	    
set xlabel "Taille du tableau"
set output "06.png"
plot "06-sum-tab[1..n].dat" using 1:(($2+$3+$4+$5+$6+$7+$8+$9+$10+$11)/10) with lines title 'Bibliothèque pthread', "06-sum-tab[1..n].dat" using 1:(($12+$13+$14+$15+$16+$17+$18+$19+$20+$21)/10) with lines title 'Bibliothèque projet'
