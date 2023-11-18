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
  int start;
} thread_args_t;

int total_prime = 0;
int numThreads;
int jump;
pthread_mutex_t mutex;

// Implement is_prime function
// If the argument is a prime number function returns 1
// If the argument is NOT a prime number function returns 0
int is_prime(int num)
{
  if (num <= 1)
  {
    return 0;
  }
  if (num == 2 || num == 3)
  {
    return 1; // 2 et 3 sont premiers
  }
  if (num % 2 == 0 || num % 3 == 0)
  {
    return 0; // Tous les nombres pairs et divisibles par 3 ne sont pas premiers
  }

  for (int i = 5; i * i <= num; i += 6)
  { 
    if (num % i == 0 || num % (i + 2) == 0)
    { 
        return 0;
    }
  }

  return 1;
}


void *thread_runner(void* thread_arg)
{
  thread_args_t *my_args = (thread_args_t *)(thread_arg);
  int count = 0;
  int max = 0;

  //printf("Thread %d start\n", my_args->thread_id);
  
  for (int num = my_args->start; num <= N ; num = num + numThreads*jump)
  {
    if(num+jump > N)  //checks not to exceed N
    {
      max = N - num;
    }
    else
    {
      max = jump;
    }
    for(int i = 0 ; i < max ; i++)
    {
      if (is_prime(num+i))
      {
        count++;
      }
    } 
  }

  //printf("%d prime numbers in thread %d\n", count, my_args->thread_id);
  pthread_mutex_lock(&mutex);
  total_prime += count;
  pthread_mutex_unlock(&mutex);
  
  //printf("Thread %d stop\n", my_args->thread_id);
  pthread_exit(NULL);
}

int main(int argc, char **argv)
{
  numThreads = NUM_THREADS;   // Dedault value 
  if(argc > 1)                    //read the number of threads
  {
    numThreads = atoi(argv[1]);
    //printf("Number of threads : %d\n", numThreads);
  }
  pthread_t threads[numThreads];
  thread_args_t threadParams[numThreads];

  jump = (numThreads%5 == 0) ? 15 : 14;
  for (int i = 0; i < numThreads; i++)
  {
    threadParams[i].thread_id = i;
    threadParams[i].start = i*jump;
    pthread_create(&threads[i], NULL, thread_runner, (void *)&threadParams[i]);
  }

  for (int i = 0; i < numThreads; ++i) 
  {
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&mutex);

  //printf("NUMBER PRIMES = %d\n", total_prime);
  exit(EXIT_SUCCESS);
}
