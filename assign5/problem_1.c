#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

pthread_mutex_t mutex;
pthread_cond_t cat_cond;
pthread_cond_t dog_cond;
pthread_cond_t bird_cond;

int num_cats;
int num_dogs;
int num_birds;
int all_animals;

volatile int cats;
volatile int dogs;
volatile int birds;

volatile int cats_play;
volatile int dogs_play;
volatile int birds_play;

volatile int sleep_flag;

pthread_t* threads;

void* cat_enter();
void* dog_enter();
void* bird_enter();

void cat_exit();
void dog_exit();
void bird_exit();

void play(void);

int main(int argc, char** argv) {
  // make sure enter correct arguments
  if (argc != 4) {
    fprintf(stderr, "ERROR: Please provide correct number of arguments (3).\n"); 
    return 1; 
  }

  // initialize stuff with nums from arguments 
  num_cats = strtol(argv[1], NULL, 10); 
  num_dogs = strtol(argv[2], NULL, 10); 
  num_birds = strtol(argv[3], NULL, 10); 
    
  // input checkign
  if (num_cats <= 0 || num_cats > 99 || num_dogs <= 0 || num_dogs > 99 || num_birds <= 0 || num_birds > 99) {
    fprintf(stderr, "ERROR: Check that arguments are valid numbers. Only 0-99 per animal allowed.\n");
    return 1;
  }
  
  all_animals = num_cats + num_dogs + num_birds;

  // initialze threads, then check for correct initalize
  threads = malloc(all_animals * sizeof(pthread_t));

  if (threads == NULL) {
    fprintf(stderr, "ERROR: malloc had an error.\n");
    return 1;
  }
 
  if (pthread_mutex_init(&mutex, NULL) || pthread_cond_init(&cat_cond, NULL) || pthread_cond_init(&dog_cond, NULL) || pthread_cond_init(&bird_cond, NULL)) {
    fprintf(stderr, "ERROR: condition variables or mutex initialization had an error.\n");
    return 1;
  }

  // intialize animals and animals_play
  cats = 0;
  dogs = 0;
  birds = 0;

  cats_play = 0;
  dogs_play = 0;
  birds_play = 0;

  sleep_flag = 1; 


  // start animals entering their threads
  int i;
  for (i = 0; i < num_cats; i++) {
    if (pthread_create(&threads[i], NULL, cat_enter, NULL)) {
      fprintf(stderr, "ERROR: cat thread %d could not be created.\n", i);
      return 1;
    }
  }
  for (i = num_cats; i < num_cats + num_dogs; i++) {
    if (pthread_create(&threads[i], NULL, dog_enter, NULL)) {
      fprintf(stderr, "ERROR: dog thread %d could not be created.\n", i);
      return 1;
    }
  }
  for (i = num_cats + num_dogs; i < all_animals; i++) {
    if (pthread_create(&threads[i], NULL, bird_enter, NULL)) {
      fprintf(stderr, "ERROR: bird thread %d could not be created.\n", i);
      return 1;
    }
  }

  
  // now wait 10 seconds for all these to do work
  sleep(10);
  sleep_flag = 0;

  // try to join back up all the threads
  for (i = 0; i < all_animals; i++) {
    if (pthread_join(threads[i], NULL)) {
      fprintf(stderr, "ERROR: Thread %d could not be joined.\n", i);
      return 1;
    }
  }

  // and print how many times each animal played
  printf("cat play = %d, ", cats_play);
  printf("dog play = %d, ", dogs_play);
  printf("bird play = %d\n", birds_play);


  return 0;
}

void* cat_enter(void) {
  while (sleep_flag) {
    pthread_mutex_lock(&mutex);
    // cats are stupid and can't play with dogs or birds
    while (dogs != 0 && birds != 0) {
      pthread_mutex_unlock(&mutex);
      pthread_cond_wait(&cat_cond, &mutex);
      pthread_mutex_lock(&mutex);
    }
    cats++;
    play();
    cats_play++;
    cat_exit();
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

void* dog_enter(void) {
  while (sleep_flag) {
    pthread_mutex_lock(&mutex);
    while (cats != 0) {
      pthread_mutex_unlock(&mutex);
      pthread_cond_wait(&dog_cond, &mutex);
      pthread_mutex_lock(&mutex);
    }
    dogs++;
    play();
    dogs_play++;
    dog_exit();
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

void* bird_enter(void) {
  while (sleep_flag) {
    pthread_mutex_lock(&mutex);
    while (cats != 0) {
      pthread_mutex_unlock(&mutex);
      pthread_cond_wait(&bird_cond, &mutex);
      pthread_mutex_lock(&mutex);
    }
    birds++;
    play();
    birds_play++;
    bird_exit();
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

void cat_exit(void) {
  cats--;
  pthread_cond_signal(&dog_cond);
  pthread_cond_signal(&bird_cond);
}

void dog_exit(void) {
  dogs--;
  pthread_cond_signal(&cat_cond);
}

void bird_exit(void) {
  birds--;
  pthread_cond_signal(&cat_cond);
}

void play(void) {
  int i;
  for (i = 0; i < 10; i++) {
    assert(cats >= 0 && cats <= num_cats);
    assert(dogs >= 0 && dogs <= num_dogs);
    assert(birds >= 0 && birds <= num_birds);
    assert(cats == 0 || dogs == 0);
    assert(cats == 0 || birds == 0);
  }
}
