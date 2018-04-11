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


/* spin_lock and spin_unlock from assign page */
struct spin_lock_t {
  volatile int current;
  volatile int waiting;
} spin_lock_t;

// locker to be used
struct spin_lock_t* locker;

void spin_lock (struct spin_lock_t *s);
void spin_unlock (struct spin_lock_t *s);

/*
 * atomic_xadd
 *
 * equivalent to atomic execution of this code:
 *
 * return (*ptr)++;
 *
 */
static inline int atomic_xadd (volatile int *ptr) {
  register int val __asm__("eax") = 1;
  asm volatile ("lock xaddl %0, %1"
  : "+r" (val)
  : "m" (*ptr)
  : "memory"
  );

  return val;
}


// implement spin_lock and _unlock
void spin_lock (struct spin_lock_t *s) {
  // use the given function and consider lock to be held when value is 1 and free when value is 0
  int a;
  a = atomic_xadd(&(s->waiting));
  while (a != s->current) { sched_yield(); }
}
void spin_unlock (struct spin_lock_t *s) {
  // now unlock by switching values
  atomic_xadd(&(s->current));
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
  // don't forget to initialize locker....
  locker = malloc(sizeof(spin_lock_t));

  // check return value of every system call/library call that might fail
  if (threads == NULL || info == NULL || tickets == NULL || entering == NULL || locker == NULL) {
    fprintf(stderr, "malloc didn't work \n");
    return 1;
  }

  locker->current = 0;
  locker->waiting = 0;

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
  tickets[i] = 1 + findMax();
  entering[i] = 0;

  int j;
  for (j = 0; j < num_threads; j++) {
    // wait until thread j receives its number:
    while (entering[j] != 0) { /* nothing */ }

    // wait until all threads with smaller numbers or with the same
    // number, but with higher priority, finish their work:
    while ((tickets[j] != 0) && (tickets[j] < tickets[i] || (tickets[j] == tickets[i] && i < j))) { /* nothing */ }
  }
}

void unlock(int i) {
  tickets[i] = 0;
}

void* Thread(void* info) {

  while (sleep_flag) {
    // int t = ((thread_info*)info)->i;
    spin_lock(locker);

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
    
    spin_unlock(locker);
  }

  return NULL;
}
