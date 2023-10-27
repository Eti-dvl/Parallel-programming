#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#define N 10000000

typedef struct
{
  int thread_id;
} thread_args_t;

int total_prime = 0;

// Implement is_prime function
// If the argument is a prime number function returns 1
// If the argument is NOT a prime number function returns 0
int is_prime(int num)
{
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
  // TODO: 1- Implement the is_prime function and test it
  // TODO: 2- Use argc, argv arguments to set the number of running threads
  // TODO: 3- Implement the thread_runner function

  printf("NUMBER PRIMES = %d\n", total_prime);
  exit(EXIT_SUCCESS);
}
