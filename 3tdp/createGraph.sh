if [ ! -d "build" ]
then 
echo "You must compile the project first."
exit 1
fi

# values_node='1 2 3 6' 
# values_part='10 20 30 40 50 60 70 80 90 100'
# nb_part=10
# nb_iter=10000
# dt=0.0001
# cmpt=0
# rm -f honorat*

# ## Création du graphe ms en fonction du nombre de noeuds (1 coeur par noeud)

# nom_fichier1="graph_nodes"
# rm -f $nom_fichier1.*
# echo -e "# $nom_fichier1 (computing)"
# echo -e "# nb_nodes \t # exec time (ms) for 6/nb_nodes ppn, $nb_iter iterations with an auto_dt and $nb_part particles" > $nom_fichier1.dat
# for i in $values_node
# do
# ((cmpt++))
# echo "module load compiler/gcc mpi/openmpi" > tmp.pbs
# echo "#PBS -l nodes=$i:ppn=`expr 6 / $i`" >> tmp.pbs
# echo "mpiexec -np 6 ./tdp/bin/main -p $nb_part -n $nb_iter" >> tmp.pbs
# qsub -l nodes=2:ppn=3,walltime=01:00:00 -N honoduro -W x=NACCESSPOLICY:SINGLEJOB tmp.pbs
# cat honoduro* 
# while [ $? -ne 0 ]
# do
# sleep 1
# cat honoduro*
# done
# cat honoduro* >> $nom_fichier1.out
# cat $nom_fichier1.out | grep 'Exec time of proc #0 (ms):' | sed "$cmpt q;d" |sed 's/^.* \([0-9]*\)$/\1/' > tmp
# value=$(head -n 1 tmp)
# echo -e "$i\t$value" >> $nom_fichier1.dat
# rm -f honoduro*
# done
# cmpt=0
# gnuplot <<EOF
# set terminal png
# set title "Temps d'execution en fonction du nombre de noeuds"
# set yrange [0:500]	    
# set ylabel "Temps d'execution (ms)"	    
# set xlabel "Nombre de noeuds (Plafrim)"
# set output '$nom_fichier1.png'
# plot '$nom_fichier1.dat' using 1:2 with lines title 'Charge totale : 6 coeurs'
# EOF
# echo -e "--->Graphic generated"
