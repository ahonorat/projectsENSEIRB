/**
 * -- bench.h --
 * fonctions utilisées pour faire les benchmark
 **/

#define BENCH_MAT_MAX_SIZE 1024

/**
 * Effectue un benchmark de la fonction process
 * sur les données data et enregistre la sortie dans filename
 * @param filename chemin vers le fichier log
 * @param flop nombre d'operations flottantes en fonction de la taille des données
 * @param process(data,i) fonction a benchmarker, data sont les données en entrée et i la taille des données
 * @param data les données en entrée (ex: la matrice) transmises à process
 **/
void bench_generic( char* filename, int iter,
                    long (*flop)(long i),
                    void (*process)(void* data, int i),
                    void* data);

// Nombre de flop du dgemm en fonction de la taille de la matrice
long flop_dgemm(long i);
// Methodes a benchmarker pour les differents dgemms
void process_myblas_dgemm_bloc_parallel(void* data, int i);
