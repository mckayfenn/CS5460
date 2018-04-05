#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int num_threads;
int num_secs;

volatile int in_cs;
volatile int sleep_flag;

volatile int* tickets;
volatile int* entering;

void lock(int);
void unlock(int);
void* Thread(void*);

typedef struct thread_info {
  int i; // customer number for thread
  int n; // thread number for thread
} thread_info;

thread_info* info;
pthread_t* threads;

int main(int argc, char **argv) {
  
  // get the arguments
  if (argc != 3) {
    fprintf(stderr, "Please give the correct arguments\n");
    return 1;
  }

  num_threads = strtol(argv[1], NULL, 10);
  num_secs = strtol(argv[2], NULL, 10);

  printf("threads: %d\n", num_threads);
  printf("seconds: %d\n", num_secs);


  threads = malloc(num_threads * sizeof(pthread_t));
  info = malloc(num_threads * sizeof(thread_info));
  tickets = malloc(num_threads * sizeof(int));
  entering = malloc(num_threads * sizeof(int));

  // check return value of every system call/library call that might fail
  if (threads == NULL || info == NULL || tickets == NULL || entering == NULL) {
    fprintf(stderr, "malloc didn't work \n");
    return 1;
  }

  // initialize arrays
  int i;
  for (i = 0; i < num_threads; i++) {
    info[i].i = i;
    info[i].n = 0;
    
    tickets[i] = 0;
    entering[i] = 0;
  }
  sleep_flag = 1;

  // create all the pthreads and check they are actually created
  for (i = 0; i < num_threads; i++) {
    if (pthread_create(&threads[i], NULL, Thread, &info[i]) != 0) {
      fprintf(stderr, "failed to create thread %d\n", i);
      return 1;
    }
    else
      printf("created thread: %d", i);
  }

  return 0;
}
