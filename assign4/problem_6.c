#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>
#include <time.h>

int num_threads;
int num_secs;

volatile int total_points = 0;
volatile int in_circle = 0;

volatile int in_cs;
volatile int sleep_flag;

void* Thread();

pthread_t* threads;

// use a mutex lock
pthread_mutex_t lock;

double rand_lim(int limit);

int main(int argc, char **argv) {
  
  // get the arguments
  if (argc != 3) {
    fprintf(stderr, "Please give the correct arguments\n");
    return 1;
  }

  num_threads = strtol(argv[1], NULL, 10);
  num_secs = strtol(argv[2], NULL, 10);

  // only call this once
  srand(time(NULL));

  threads = malloc(num_threads * sizeof(pthread_t));

  // check return value of every system call/library call that might fail
  if (threads == NULL) {
    fprintf(stderr, "malloc didn't work \n");
    return 1;
  }

  if (pthread_mutex_init(&lock, NULL) != 0) {
    fprintf(stderr, "pthread_mutex_init failed\n");
    return 1;
  }

  sleep_flag = 1;
  int i;
  for (i = 0; i < num_threads; i++) {
    // create the threads and run them
    if (pthread_create(&threads[i], NULL, Thread, NULL)) {
      fprintf(stderr, "Thread %d could not be created\n", i);
      return 1;
    }
  }

  // continue running
  sleep(num_secs);
  
  sleep_flag = 0;

  // join threads and print their info
  for (i = 0; i < num_threads; i++) {
    if (pthread_join(threads[i], NULL)) {
      fprintf(stderr, "Thread %d couldn't be joined\n", i);
      return 1;
    }

  }

  // calculate pi
  double pi = 4 * (double)(in_circle) / (double)(total_points);
  printf("approximating pi to: %lf\n", pi);

  return 0;
}

void* Thread() {
  
  while (sleep_flag) {
    // get a random x and y between 0 and 2
    double rand_x = (double)rand() / (double)(RAND_MAX);
    double rand_y = (double)rand() / (double)(RAND_MAX);

    pthread_mutex_lock(&lock);
      // critical seciton

      // increase total
      total_points++;

      // and check if the point is in the circle
      if (((rand_x * rand_x) + (rand_y * rand_y)) <= 1)
        in_circle++;

    pthread_mutex_unlock(&lock);
  }

  return NULL;
}
