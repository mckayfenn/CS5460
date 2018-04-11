#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

int num_threads;
int num_secs;

volatile int in_cs;
volatile int sleep_flag;

volatile int* tickets;
volatile int* entering;

int findMax();
void lock(int);
void unlock(int);
void* Thread(void*);

typedef struct thread_info {
  int i; // customer number for thread
  int n; // thread number for thread
} thread_info;

thread_info* info;
pthread_t* threads;


/* code from assign page */
void mfence (void) {
  asm volatile ("mfence" : : : "memory");
}

int main(int argc, char **argv) {
  
  // get the arguments
  if (argc != 3) {
    fprintf(stderr, "Please give the correct arguments\n");
    return 1;
  }

  num_threads = strtol(argv[1], NULL, 10);
  num_secs = strtol(argv[2], NULL, 10);

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
  // pass them to the Thread function too
  for (i = 0; i < num_threads; i++) {
    if (pthread_create(&threads[i], NULL, Thread, &info[i]) != 0) {
      fprintf(stderr, "failed to create thread %d\n", i);
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

    // print how many times each thread entered the critical dd
    printf("Thread %d entered critical section %d times.\n", i, (int)(info[i].n));
  }

  return 0;
}

int findMax() {
  int current_max = 0;
  int i;
  for (i = 0; i < num_threads; i++) {
    if (tickets[i] > current_max)
      current_max = tickets[i];
  }
  return current_max;
}

void lock(int i) {
  entering[i] = 1;

  // replace function call
  int cur_max = 0;
  int j;
  for (j = 0; j < num_threads; j++) {
    if (tickets[j] > cur_max)
      cur_max = tickets[j];
  }

  tickets[i] = 1 + cur_max;
  entering[i] = 0;

  for (j = 0; j < num_threads; j++) {

    // use mfence() here to make sure that the tickets and entering are both at the same place and up to date for the while loops they are doing.
    mfence();

    // wait until thread j receives its number:
    while (entering[j] != 0) { /* nothing */ }

    // wait until all threads with smaller numbers or with the same
    // number, but with higher priority, finish their work:
    while ((tickets[j] != 0) && (tickets[j] < tickets[i] || (tickets[j] == tickets[i] && i < j))) { /* nothing */ }
  }
}

void unlock(int i) {
  tickets[i] = 0;

  // use mfence() here to make sure that the tickets are reset correctly.
  mfence();
}

void* Thread(void* info) {

  while (sleep_flag) {
    int t = ((thread_info*)info)->i;
    lock(t);

    // critical section
    (((thread_info*)info)->n)++;
    assert (in_cs == 0);
    in_cs++;
    assert (in_cs == 1);
    in_cs++;
    assert (in_cs == 2);
    in_cs++;
    assert (in_cs == 3);
    in_cs = 0;
    
    unlock(t);
  }

  return NULL;
}
