#include <stdio.h>
#include <stdlib.h>

int g;


int fib(int n)
{
  int res = 0;

  static int cnt = 0;
  cnt += 1;

  if (n <= 1) 
  {
    res = n; 
  }
  else 
  {
    res = fib(n-1) + fib(n-2);
  }
  return res;
}

int main(int argc, char **argv) 
{
  char *str;
  char str2[] = "a string";
  int f;
  
  str = malloc(100*sizeof(char));
  if (!str) 
  {
    printf("Alloc failed\n");
    exit(1);
  }
  
  sprintf(str, "this is a character string which contains `%s`\n", str2); 
  
  printf("%s\n", str);
  g = fib(10);
  f = fib(13);
  printf("By the way, fib(10) = %d, fib(13) = %d\n", g, f);
  
  free(str);
  
  return 0;
}