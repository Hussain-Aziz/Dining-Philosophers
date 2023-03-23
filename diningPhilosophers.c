#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

// define some constants
#define PHILOSOPHER_NUM 5
#define MAX_MEALS 10 
#define RUN_TIME 5
#define MAX_THINK_EAT 4.0

// define the possible states of the philosopher
typedef enum {THINKING, EATING, HUNGRY} PhilosopherState;

// some forward definitions
void* Philosopher(void* number);
double Random(double min, double max);

// declare some global variables
PhilosopherState philosopherState[PHILOSOPHER_NUM];
int mealsEaten[PHILOSOPHER_NUM];

int main() 
{
   // an array to hold the thread objects
   pthread_t philosopherThreads[PHILOSOPHER_NUM];

   // loop to create the threads
   for(int i = 0; i < PHILOSOPHER_NUM; i++)
   {
      // use a pointer to pass the num as void* param
      int *arg = malloc(sizeof(int)); *arg = i;

      // create the thread
      pthread_create(&philosopherThreads[i], 0, Philosopher, arg);
   }

   // let threads run
   sleep(RUN_TIME);

   // cancel threads since our total runtime is over
   for(int i = 0; i < PHILOSOPHER_NUM; i++)
   {
      pthread_cancel(philosopherThreads[i]);
   }

   // compute the data
   int min = __INT_MAX__, max = 0, totalMeals = 0;

   for(int i = 0; i <= PHILOSOPHER_NUM; i++)
   {
      totalMeals += mealsEaten[i];

      if(mealsEaten[i] < min) min = mealsEaten[i];
      if(mealsEaten[i] > max) max = mealsEaten[i];
   }

   // print the data
   printf("min: %i\nmax: %i\naverage: %f\n", min, max, (double)totalMeals/PHILOSOPHER_NUM);

   return 0;
}

void* Philosopher(void* num)
{
   // cast the parameter to the int we provided earlier
   int number = *((int*)num);

   // initalize the place in the array to 0
   mealsEaten[number] = 0;

   // make sure only 10 meals are eaten
   while(mealsEaten[number] < MAX_MEALS)
   {
      // start thinking
      philosopherState[number] = THINKING;
      sleep(Random(1, MAX_THINK_EAT));

      philosopherState[number] = HUNGRY;
      //TODO: check if chopsticks available

      // start eating
      philosopherState[number] = EATING;
      sleep(Random(1, MAX_THINK_EAT));
      // assumtion: eating only counts if they completely finish eating
      mealsEaten[number] += 1;
   }

   // since we are done, exit
   pthread_exit(NULL);
}

double Random(double min, double max)
{
   return min + ((double) rand() / RAND_MAX) * (max - min);
}