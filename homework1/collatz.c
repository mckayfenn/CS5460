/*
 * ----------------------------------------------------
 * McKay Fenn, u0939404
 * collatz.c for CS5460 Homework #1 problem 3.21
 * -----------------------------------------------------
 *
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  // first do error checking on the input
  if (argc != 2) {
    fprintf(stderr, "%s", "ERROR: Please provide a single argument.\n");
    return 0;
  }
  char *ptr;
  long value; 
  value = strtol(argv[1], &ptr, 10);
  if (value <= 0) {
    fprintf(stderr, "%s", "ERROR: Please input a single positive integer.\n");
    return 0;
  }


  pid_t pid;
  pid = fork();

  // child process do work here
  if (pid == 0) {
    printf("%lu", value);
    while (value != 1) {
      printf("%s", ", ");
      
      if (value & 1) // value is odd
        value = 3 * value + 1;
      else // value is even
        value = value / 2; 

      printf("%lu", value);
    }
    printf("%s", ".\n");
  }
  // parent process, wait for child 
  else {
    wait(NULL);

  }

  return 0;
}
