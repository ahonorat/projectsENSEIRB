# se_emk #

Création d'une bibliothèque de threads en C
(projet de système d'exploitation deuxième année Enseirb-Matmeca)

## Compilation ##

###Règles du Makefile (situé à la racine)###

+ make : contruit la bibliothèque de threads, ainsi que le programme d'exemple fourni
+ make lib : construit uniquement la bibliothèque
+ make tests : construits les exécutables de tous les tests avec la bibliothèque du projet
+ rajouter OPT=-DUSE_PTHREAD à la commande make tests pour compiler les tets avec la bibliothèque p_thread


###Configuration de la compilation###

Il est possible d'éditer le fichier src/conf.make afin de changer la variable DEBUG à NO pour compiler sans symboles de débuggage ni vérification valgrind. Par ailleurs notez qu'il n'existe pas de règle d'installation de la librairie ; la compilation des programmes de tests récupère le chemin absolu de la librarie et évite ainsi de la copier dans /usr/lib (ou autre) et/ou d'éditer le LD_LIBRARY_PATH. Attention par conséquent à ne pas déplacer la librairie après l'avoir compilée.

## Scripts de tests ##

Le script actuel run_tests.sh lance tous les binaires de tests fournis par M. Goglin ainsi que d'autres créés par l'équipe, avec p_thread et avec thread.so. Une comparaison entre les deux versions est effectuée pour chaque test puis un diff des résultats et un diff de valgrind est affiché. La génération/configuration peut être faite dans bench/ .
