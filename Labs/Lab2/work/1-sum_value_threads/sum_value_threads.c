#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#ifndef NUM_THREADS
#define NUM_THREADS 4
#endif

typedef struct
{
  int thread_id;
} thread_args_t;

long long int shared = 0;
// Mutex
pthread_mutex_t myMutex;

void *thread_runner(void* thread_arg)
{
  thread_args_t *my_args = (thread_args_t *)(thread_arg);

  printf("Thread %d start\n", my_args->thread_id);

  pthread_mutex_lock(&myMutex);
  for (int i = 0; i < 1000000000; ++i)
  {  
    shared += 1;
  }
  pthread_mutex_unlock(&myMutex);
  
  printf("Thread %d stop\n", my_args->thread_id);
  return (void *)0;
}

int main(int argc, char **argv)
{
  pthread_t threads[NUM_THREADS];
  thread_args_t *my_args;

  // Initialiser le mutex
  if (pthread_mutex_init(&myMutex, NULL) != 0) 
  {
      perror("Mutex initialization failed");
      return 1;
  }
  
  my_args = calloc(NUM_THREADS, sizeof(thread_args_t));

  for (int i = 0; i < NUM_THREADS; ++i)
  {
      my_args[i].thread_id = i;
      pthread_create(&threads[i], NULL, thread_runner, (void *)&my_args[i]);
  }

  for (int i = 0; i < NUM_THREADS; ++i) {
      pthread_join(threads[i], NULL);
  }

  // Détruire le mutex
  if (pthread_mutex_destroy(&myMutex) != 0) 
  {
      perror("Mutex destruction failed");
      return 1;
  }

  printf("SUM = %lld\n", shared);
  exit(EXIT_SUCCESS);
}