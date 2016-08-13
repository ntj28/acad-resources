/*
 * CS460: Operating Systems
 * Jim Plank / Rich Wolski
 * dphil_5_book.c -- Dining philosophers solution #5 -- the book's solution
 */


#include <stdio.h>
#include <pthread.h>
#include "dphil.h"


#define THINKING 0
#define HUNGRY 1
#define EATING 2

typedef struct {
  pthread_mutex_t *mon;
  pthread_cond_t **cv;
  int *state;
  int phil_count;
} Phil;


test(Phil *pp, int n)
{
  int phil_count;
  
  phil_count = pp->phil_count;
  if (pp->state[(n+(phil_count-1))%phil_count] != EATING &&
      pp->state[n]       == HUNGRY &&
      pp->state[(n+1)%phil_count] != EATING) {
    pp->state[n] = EATING;
    pthread_cond_signal(pp->cv[n]);
  }
}

void pickup(Phil_struct *ps)
{
  Phil *pp;

  pp = (Phil *) ps->v;
  
  pthread_mutex_lock(pp->mon);
  pp->state[ps->id] = HUNGRY;
  test(pp, ps->id);
  while (pp->state[ps->id] != EATING) {
    pthread_cond_wait(pp->cv[ps->id], pp->mon);
  }
  pthread_mutex_unlock(pp->mon);
}

void putdown(Phil_struct *ps)
{
  Phil *pp;
  int phil_count;

  pp = (Phil *) ps->v;
  phil_count = pp->phil_count;

  pthread_mutex_lock(pp->mon);
  pp->state[ps->id] = THINKING;
  test(pp, (ps->id+(phil_count-1))%phil_count);
  test(pp, (ps->id+1)%phil_count);
  pthread_mutex_unlock(pp->mon);
}

void *initialize_v(int phil_count)
{
  Phil *pp;
  int i;

  pp = (Phil *) malloc(sizeof(Phil));
  pp->phil_count = phil_count;
  pp->mon = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pp->cv = (pthread_cond_t **) malloc(sizeof(pthread_cond_t *)*phil_count);
  if (pp->cv == NULL) { perror("malloc"); exit(1); }
  pp->state = (int *) malloc(sizeof(int)*phil_count);
  if (pp->state == NULL) { perror("malloc"); exit(1); }
  pthread_mutex_init(pp->mon, NULL);
  for (i = 0; i < phil_count; i++) {
    pp->cv[i] = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    if (pp->cv[i] == NULL) { perror("malloc"); exit(1); }
    pthread_cond_init(pp->cv[i], NULL);
    pp->state[i] = THINKING;
  }

  return (void *) pp;
}
