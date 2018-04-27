#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex;
pthread_cond_t cat_cond;
pthread_cond_t dog_cond;
pthread_cond_t bird_cond;

int num_cats;
int num_dogs;
int num_birds;
int all_animals;


int main(int argc, char** argv) {
  if (argc != 4) {
    fprintf(stderr, "Please provide correct number of arguments (3).\n");
    return 1;
  }
 

  return 0;
}
