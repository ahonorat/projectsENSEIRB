if [ ! -d "build" ]
then 
echo "You must compile the project first."
exit 1
fi

mat_sizes='100 200 300 400 500 600 700 800 900 1000'
mat_size=500
total_comb=24
values_node='1 2 3 4 5 6'
nb_nodes=(8 3 4 5 3 1)
ppn1='1 2 3 4 5 6 7 8'
ppn2='5 6 7'
ppn3='3 5 6 7'
ppn4='4 5 6 7 8'
ppn5='5 6 7'
ppn6='6'
values_ppn=($ppn1 $ppn2 $ppn3 $ppn4 $ppn5 $ppn6)
nb_iter=10
cmpt=0
rm -f honorat*

## Création du graphe ms en fonction du nombre de noeuds

nom_fichier="graph_procs"
rm -f $nom_fichier.*
echo -e "# $nom_fichier (computing)"
echo -e "# nb_proc \t # exec time (ms) for matrix length = $mat_size" > $nom_fichier.dat
for i in $values_node
do
    for j in 1..${nb_nodes[$i]}
    do
	((cmpt++))
	echo "module load compiler/intel mpi/openmpi" > tmp.pbs
	echo "#PBS -l nodes=$i:ppn=${values_ppn[$cmpt]}" >> tmp.pbs
	echo "mpiexec -np $(($i * ${values_ppn[$cmpt]})) ./tdp/3tdp/bin/bench $mat_size $nb_iter" >> tmp.pbs
	qsub -l walltime=01:00:00 -N honorat tmp.pbs
	cat honorat* 
	while [ $? -ne 0 ]
	do
	    sleep 1
	    cat honorat*
	done
	cat honorat* >> $nom_fichier.out
	value=$(cat $nom_fichier.out)
	echo -e "$(($i * ${values_ppn[$cmpt]}))\t$value" >> $nom_fichier.dat
	rm -f honorat*
    done
done
cmpt=0
gnuplot <<EOF
set terminal png
set title "Temps d'execution en fonction du nombre de processus"
set ylabel "Temps d'execution (ms)"	    
set xlabel "Nombre de noeuds (Plafrim)"
set output '$nom_fichier.png'
plot '$nom_fichier.dat' using $1:$3 with lines title 'Taille matric : $mat_size'
EOF
echo -e "--->Graphic generated"
