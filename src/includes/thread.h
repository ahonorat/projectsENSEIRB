#ifndef __THREAD_H__
#define __THREAD_H__
#include <pthread.h>

#ifndef USE_PTHREAD

/* identifiant de thread
 * NB: pourra être un entier au lieu d'un pointeur si ca vous arrange,
 *     mais attention aux inconvénient des tableaux de threads
 *     (consommation mémoire, cout d'allocation, ...).
 */
typedef struct thread * thread_t;

/* recuperer l'identifiant du thread courant.
 */
extern thread_t thread_self(void);

/* creer un nouveau thread qui va exécuter la fonction func avec l'argument funcarg. 
 * renvoie 0 en cas de succès, -1 en cas d'erreur.
 */
extern int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg);

/* idem mais permet de préciser le type d'ajout à la liste des ready
 * 0 pour FIFO, 1 pour LIFO
 */

extern int thread_create_a(thread_t *newthread, void *(*func)(void *), void *funcarg, int add_type);

/* passer la main à un autre thread.
 */
extern int thread_yield(void);

/* attendre la fin d'exécution d'un thread.
 * la valeur renvoyée par le thread est placée dans *retval.
 * si retval est NULL, la valeur de retour est ignorée.
 */
extern int thread_join(thread_t thread, void **retval);

/* terminer le thread courant en renvoyant la valeur de retour retval.
 * cette fonction ne retourne jamais.
 *
 * L'attribut noreturn aide le compilateur à optimiser le code de
 * l'application (élimination de code mort). Attention à ne pas mettre
 * cet attribut dans votre interface tant que votre thread_exit()
 * n'est pas correctement implémenté (il ne doit jamais retourner).
 */
extern void thread_exit(void *retval);

/* autorise l'annulation ou non d'un thread, par défaut acceptée 
 * les états acceptés sont PTHREAD_CANCEL_ENABLE
 * et PTHREAD_CANCEL_ENABLE
 * retourne erreur sinon
 */
extern int thread_setcancelstate(int state, int *oldstate);

/* annule le thread en cours au prochain yield sur celui-ci */

extern int thread_cancel(thread_t thread);

#else /* USE_PTHREAD */

/* Si on compile avec -DUSE_PTHREAD, ce sont les pthreads qui sont utilisés */
# ifndef _GNU_SOURCE
#  define _GNU_SOURCE /* for pthread_yield() */
# endif

# define thread_t pthread_t
# define thread_self pthread_self
# define thread_create(th, func, arg) pthread_create(th, NULL, func, arg)
# define thread_create_a(th, func, arg, adding_type) pthread_create(th, NULL, func, arg)
# define thread_yield pthread_yield
# define thread_join pthread_join
# define thread_exit pthread_exit
# define thread_setcancelstate pthread_setcancelstate
# define thread_cancel pthread_cancel

#endif /* USE_PTHREAD */

#endif /* __THREAD_H__ */
