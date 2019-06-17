# se_emk #

Création d'une bibliothèque de threads en C
(projet de système d'exploitation deuxième année Enseirb-Matmeca)

## Compilation ##

###Règles du Makefile (situé à la racine)###

+ make : contruit la bibliothèque de threads, ainsi que le programme d'exemple fourni
+ make lib : construit uniquement la bibliothèque
+ make tests : construit les exécutables de tous les tests avec la bibliothèque du projet
+ rajouter OPT=-DUSE_PTHREAD à la commande make tests pour compiler les tets avec la bibliothèque p_thread


###Configuration de la compilation###

Il est possible d'éditer le fichier src/conf.make afin de changer la variable DEBUG à NO pour compiler sans symboles de débuggage ni vérification valgrind. Par ailleurs notez qu'il n'existe pas de règle d'installation de la librairie ; la compilation des programmes de tests récupère le chemin absolu de la librarie et évite ainsi de la copier dans /usr/lib (ou autre) et/ou d'éditer le LD_LIBRARY_PATH. Attention par conséquent à ne pas déplacer la librairie après l'avoir compilée, car il s'agit bien d'une librairie dynamique.

## Scripts de tests ##

###Exécution de tous les tests

Le script run_tests.sh lance tous les binaires de tests fournis par M. Goglin ainsi que d'autres créés par l'équipe, avec p_thread et avec thread.so. Une comparaison entre les deux versions est effectuée pour chaque test puis un diff des résultats est affiché. Tous les tests sont effectués avec comme variables l'entier 10.

###Vérification mémoires###

Le script valgrind_tests.sh lance tous les binaires de tests (idem run_tests.sh) avec cette fois-ci valgrind. Le diff affiché correspond au diff des résultats valgrind. Le script met un certain temps à s'exécuter.

###Création de graphiques###

La génération de graphes peut être faite dans bench/ grâce au script create_graph.sh. Celui génère les graphique (sortie en .png) de cinq tests différents, correspondant à chaque fois au temps d'exécution de p_thread et celui de la bibliothèque du projet, en fonction d'une variable indiquée (le plus souvent le nombre de threads). Une moyenne sur 10 exécution est faite afin d'éviter la présence de résultats non significatifs (car perturbés par des interruptions ou autre). 
* Attention : nécessite gnuplot et un terminal associé (gnuplot-x11 par exemple). *
