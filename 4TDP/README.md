TDP4
====

ahonorat -- ekeryel-even

## Infos de base.

Seuls le Makefile et img.mpi-thread.c ont été modifiés.
Des headers ont été rajouté dans le dossier include/ pour
la gestion des piles, et pour les define propre à notre code.
(et aussi type.h a été modifié pour permettre des longueurs 
de chemin supérieures à 32 caractères)

Fichiers/dossiers importants

.
|-- README.md (ici)
|-- TDP4.pdf (rapport)
|-- automate.png (et .dot)
|-- LancerRayons
  |-- src
  |-- res (contient 3 graphes)
  |-- buildPlafrim.sh
  |-- createGraph.sh


## Compilation/exécution

### Sous Plafrim

*4tdp-hk* doit être situé à la racine de votre espace personnel
afin de gérer correctement les chemins dans les ```qsub```.

Allez dans LancerRayons. Pour la compilation :
```./buildPlafrim.sh```
Pour la création automatique de graphe (après compilation) :
```./createGraph.sh```

### Ailleurs

Exemple de compilation et d'exécution depuis LancerRayons/src/ 
```make && mpiexec -np 3 lanceur ../scn/exemple/test```
