set title "Speedup en fonction de la taille du probleme"
set ylabel "Speedup par rapport à 1 seul coeur"	    
set xlabel "Taille du probleme"
set terminal png
set output 'para.png'
plot 'res/para.dat' using 1:($2/$3) with lines title '4 coeurs', 'res/para.dat' using 1:($2/$4) with lines title '16 coeurs'
