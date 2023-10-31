#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#define N 10000000
#define NUM_THREADS 4

typedef struct
{
  int thread_id;
} thread_args_t;

int total_prime = 0;

// Implement is_prime function
// If the argument is a prime number function returns 1
// If the argument is NOT a prime number function returns 0
int is_prime(int num){
    if (num % 2 == 0){
        return 0; // 2 et tous les nombres divisables par 2 ne sont pas premiers
    }

    if (num == 3){
      return 1; // 3 est premier
    }

    for (int i = 5; i * i <= num; i += 6) { // on détermine la racine carré du nombre
        if (num % i == 0 || num % (i + 2) == 0) { // On vérifie les diviseurs jusqu'à la racine carré obtenu et on teste si c'est premier ou non.
            return 0;
        }
    }

    return 1;
}


void *thread_runner(void* thread_arg)
{
  thread_args_t *my_args = (thread_args_t *)(thread_arg);

  printf("Thread %d start\n", my_args->thread_id);
  
  // TODO: count the number of prime numbers
  
  printf("Thread %d stop\n", my_args->thread_id);
  return (void *)0;
}

int main(int argc, char **argv)
{
  int numThreads = NUM_THREADS;  // Dedault value
  pthread_t threads[numThreads];
  thread_args_t threadParams[numThreads];

  //read the number of threads
  if(argc > 1)
  {
    numThreads = atoi(argv[1]);
    printf("Number of threads : %d\n", numThreads);
  }

  for (int i = 0; i < numThreads; i++) 
  {
    threadParams[i].thread_id = i;
    pthread_create(&threads[i], NULL, thread_runner, (void *)&threadParams[i]);
  }


  for (int i = 0; i < NUM_THREADS; ++i) 
  {
    pthread_join(threads[i], NULL);
  }

  printf("NUMBER PRIMES = %d\n", total_prime);
  exit(EXIT_SUCCESS);
}
