if [ ! -e "./src/lanceur" ]
then 
echo "You must compile the project first."
exit 1
fi

values_node='1 2 3 4' # 5 6
nb_nodes=(8 3 4 5) # 3 1
ppn1='1 2 3 4 5 6 7 8'
ppn2='5 6 7'
ppn3='3 5 6 7'
ppn4='4 5 6 7 8'
#ppn5='5 6 7'
#ppn6='6'
values_ppn=($ppn1 $ppn2 $ppn3 $ppn4) # $ppn5 $ppn6
nb_iter=5
cmpt=0

## Création du graphe ms en fonction du nombre de processus

nom_fichier="graph_procs"
rm -f $nom_fichier.*
echo -e "# $nom_fichier (computing)"
echo -e "# nb_proc \t # exec time (ms)" > tmp
for i in $values_node
do
    for j in $(seq 1 ${nb_nodes[$(($i - 1))]})
    do
	echo "#PBS -l nodes=$i:ppn=${values_ppn[$cmpt]}" > tmp.pbs
	echo "module load compiler/gcc mpi/openmpi/current" >> tmp.pbs
	echo "mpiexec -np $(($i * ${values_ppn[$cmpt]})) ./4tdp-hk/LancerRayons/src/lanceur ./4tdp-hk/LancerRayons/scn/exemple/test" >> tmp.pbs
	rm -f $nom_fichier.out
	for k in $(seq 1 $nb_iter)
	do
	    rm -f honorat*
	    qsub -l walltime=00:10:00 -N honorat tmp.pbs
	    cat honorat* 
	    while [ $? -ne 0 ]
	    do
		sleep 2
		cat honorat*
	    done
	    cat honorat* | grep 'image' | grep -Eo '[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?' | head -n 1 >> $nom_fichier.out
	done
	cat $nom_fichier.out | tr '\n' '\t' > $nom_fichier.bis.out 
	value=$(head -n 1 $nom_fichier.bis.out)    
	echo -e "$(($i * ${values_ppn[$cmpt]}))\t$value" >> tmp
	((cmpt++))
    done
done
sort -k1,1 -n tmp > $nom_fichier.dat
gnuplot <<EOF
load 'dataGraph.gp'
EOF
echo -e "--->Graphic generated"


rm -f tmp
rm -f tmp.pbs
rm -f *.out
rm -f honorat*
