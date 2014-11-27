if [ ! -d "build" ]
then 
echo "You must compile the project first."
exit 1
fi

mat_sizes='250 500 750 1000 1250 1500 1750 2000 2250 2500 2750 3000'
mat_size=2500
values_node='1 2 3 4 5 6'
value_node='1 2 4'
nb_nodes=(8 3 4 5 3 1)
ppn1='1 2 3 4 5 6 7 8'
ppn2='5 6 7'
ppn3='3 5 6 7'
ppn4='4 5 6 7 8'
ppn5='5 6 7'
ppn6='6'
values_ppn=($ppn1 $ppn2 $ppn3 $ppn4 $ppn5 $ppn6)
nb_iter=5
cmpt=0
rm -f honorat*

## Création du graphe ms en fonction du nombre de processus

nom_fichier="graph_procs"
rm -f $nom_fichier.*
echo -e "# $nom_fichier (computing)"
echo -e "# nb_proc \t #  matrix size \t # exec time (ms)" > tmp
for i in $values_node
do
    for j in $(seq 1 ${nb_nodes[$(($i - 1))]})
    do
	echo "module load compiler/intel mpi/openmpi" > tmp.pbs
	echo "#PBS -l nodes=$i:ppn=${values_ppn[$cmpt]}" >> tmp.pbs
	echo "mpiexec -np $(($i * ${values_ppn[$cmpt]})) ./tdp3-honorat/bin/bench $mat_size $nb_iter" >> tmp.pbs
	qsub -l walltime=00:10:00 -N honorat tmp.pbs
	cat honorat* 
	while [ $? -ne 0 ]
	do
	    sleep 5
	    cat honorat*
	done
	cat honorat* | grep '^[0-9]' > $nom_fichier.out
	value=$(cat $nom_fichier.out)
	echo -e "$(($i * ${values_ppn[$cmpt]}))\t$value" >> tmp
	rm -f honorat*
	((cmpt++))
    done
done
sort -k1,1 -n tmp > $nom_fichier.dat
gnuplot <<EOF
set terminal png
set title "Temps d'execution en fonction du nombre de processus"
set ylabel "Temps d'execution (ms)"	    
set xlabel "Nombre de noeuds (Plafrim)"
set output '$nom_fichier.png'
plot '$nom_fichier.dat' using 1:3 with lines title 'Taille matrice : $mat_size'
EOF
echo -e "--->Graphic generated"

## Création du graphe ms en fonction de la répartition des processus

values_node='1 2 4'
echo -e "# $nom_fichier (computing)"
echo -e "# matrix size \t # exec time (ms)" > tmp
for i in $values_node
do
    nom_fichier="graph_sizes_n$i_ppn$((4 / $i))"
    rm -f $nom_fichier.*
    echo "module load compiler/intel mpi/openmpi" > tmp.pbs
    echo "#PBS -l nodes=$i:ppn=$((4 / $i))" >> tmp.pbs
    for j in $mat_sizes
    do
	echo "mpiexec -np 4 ./tdp3-honorat/bin/bench $j $nb_iter" >> tmp.pbs
    done
    qsub -l walltime=01:00:00 -N honorat tmp.pbs
    cat honorat* 
    while [ $? -ne 0 ]
    do
	sleep 5
	cat honorat*
    done
    cat honorat* | grep '^[0-9]' > tmp
    sort -k1,1 -n tmp > $nom_fichier.dat
    rm -f honorat*
done
nom_fichier="graph_sizes"
gnuplot <<EOF
set terminal png
set title "Temps d'execution en fonction de la taille des matrices"
set ylabel "Temps d'execution (ms)"	    
set xlabel "Taille des matrices carrés (nb_row = nb_col)"
set output '$nom_fichier.png'
plot '$nom_fichier_n1_ppn4.dat' using 1:2 with lines title '1 noeud, 4 ppn', '$nom_fichier_n2_ppn2.dat' using 1:2 with lines title '2 noeuds, 2 ppn', '$nom_fichier_n4_ppn1.dat' using 1:2 with lines title '4 noeuds, 1 ppn'
EOF
echo -e "--->Graphic generated"


rm -f tmp
rm -f tmp.pbs
rm -f *.out
