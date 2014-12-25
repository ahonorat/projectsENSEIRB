TDP4
====

ahonorat -- ekeryel-even

Seuls le Makefile et img.mpi-thread.c ont été modifiés.
Des headers ont été rajouté dans le doccier include/ pour
la gestion des piles, et pour les define propre à notre code.

Exemple d'exécution depuis src/ 
```
make && mpiexec -np 3 lanceur ../scn/exemple/test
```
