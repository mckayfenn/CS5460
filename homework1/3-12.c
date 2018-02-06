#include <stdio.h>
#include <unistd.h>

int main() {
  int i;
  /*
  for (i = 0; i < 4; i++)
    fork();
  */
  fork();
  fork();
  fork();

  sleep(20);
  return 0;
}
