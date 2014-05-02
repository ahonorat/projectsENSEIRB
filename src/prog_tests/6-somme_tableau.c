#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "../includes/thread.h"

#define MAX 1000000

int tableau[MAX];

struct deuxint{
  int x,y;
};

static void * sum(void *_value)
{
  thread_t th, th2;
  int err, mid;
  void *res = NULL, *res2 = NULL;
  
  struct deuxint *tab = (struct deuxint *) _value;

  if ( tab->x == tab->y){
    return (void*) tableau[tab->x];
  }
  
  mid = tab->x + (tab->y - tab->x)/2;
  
  struct deuxint value1,value2;

  value1.x = tab->x;
  value1.y = mid;
  
  value2.x = mid+1;
  value2.y = tab->y; 

  err = thread_create(&th, sum, (void*)&value1);
  assert(!err);

  err = thread_create(&th2, sum, (void*)&value2);
  assert(!err);

  err = thread_join(th, &res);
  assert(!err);
  err = thread_join(th2, &res2);
  assert(!err);

  return (void*)((int) res + (int) res2);
}

int main(int argc, char *argv[])
{
  int res, taille;
  if (argc < 2) {
    printf("argument manquant: entier x , taille du tableau a trier\n");
    return -1;
  }
  
  taille = atoi(argv[1]);

  if (taille >MAX){
    printf("trop grand, max a %d\n",MAX);
  }

  int i;
  srand(time(NULL));
  int s = 0;
  for (i = 0; i < taille; i++){
    tableau[i] = rand()%10;
    s += tableau[i];
  }
  
  struct deuxint ext;
  ext.x = 0;
  ext.y = taille;

  res = (int) sum((void *)&ext);
  printf("la somme thread vaut = %d\n", res);
  printf("la somme vaut = %d\n", res);

  return 0;
}
