# se_emk #

Création d'une bibliothèque de threads en C
(projet de système d'exploitation deuxième année Enseirb-Matmeca)

## Compilation ##

###Règles du Makefile (situé à la racine)###

+ make : contruit la bibliothèque de threads, ainsi que le programme d'exemple fourni
+ make lib : construit uniquement la bibliothèque
+ make tests : construits les exécutables de tous les tests fournis
+ rajouter OPT=-DUSE_PTHREAD dans les commandes pour utiliser la bibliothèque p_thread


###Configuration de la compilation###

Il est possible d'éditer le fichier src/conf.make afin de changer la variable DEBUG à NO pour compiler sans symboles de débuggage ni vérification valgrind

## Scripts de tests ##

Le script actuel run_tests.sh lance tous les binaires de tests fournis par M. Goglin, avec p_thread et avec thread.so. Une comparaison entre les deux versions est effectuée pour chaque test puis un diff des résultats et un diff de valgrind est affiché. La génération/configuration de graphiques n'est pas encore effectuée.