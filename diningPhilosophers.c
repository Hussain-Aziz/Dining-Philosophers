// run on linux using this command 
// gcc -pthread -o dp dp.c
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

// define some constants
#define PHILOSOPHER_NUM 5
#define MAX_MEALS 10 
#define RUN_TIME 60
#define MAX_THINK_EAT 4.0

// some forward definitions
void* philosopher(void* number);
double get_random(double min, double max);
void pickup_forks(int philosopher_number);
void return_forks(int philosopher_number);
int get_left_chopstick_num(int philosopher_number);
int get_right_chopstick_num(int philosopher_number);

// define global variable
int meals_eaten[PHILOSOPHER_NUM];
int chopsticks[PHILOSOPHER_NUM];
pthread_cond_t conditions[PHILOSOPHER_NUM];
pthread_mutex_t mutexs[PHILOSOPHER_NUM];

int main() 
{
   // init the arrays
   for(int i = 0; i < PHILOSOPHER_NUM; i++)
   {
      // make chopsticks available
      chopsticks[i] = 1;

      // initalize the condition variables and mutexes
      pthread_cond_init(&conditions[i], NULL);
      pthread_mutex_init(&mutexs[i], NULL);
   }

   // an array to hold the thread objects
   pthread_t philosopherThreads[PHILOSOPHER_NUM];

   // loop to create the threads
   for(int i = 0; i < PHILOSOPHER_NUM; i++)
   {
      // use a pointer to pass the num as void* param
      int *arg = malloc(sizeof(int)); *arg = i;

      // create the thread
      pthread_create(&philosopherThreads[i], 0, philosopher, arg);
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

   for(int i = 0; i < PHILOSOPHER_NUM; i++)
   {
      printf("%i: %i\n", i, meals_eaten[i]);
      totalMeals += meals_eaten[i];

      if(meals_eaten[i] < min) min = meals_eaten[i];
      if(meals_eaten[i] > max) max = meals_eaten[i];
   }

   // print the data
   printf("min: %i\nmax: %i\naverage: %f\n", min, max, (double)totalMeals/PHILOSOPHER_NUM);

   return 0;
}

void* philosopher(void* philosopher_number_variable)
{
   // cast the parameter to the int we provided earlier
   int philosopher_number = *((int*)philosopher_number_variable);

   // initalize the place in the array to 0
   meals_eaten[philosopher_number] = 0;

   // make sure only 10 meals are eaten
   while(meals_eaten[philosopher_number] < MAX_MEALS)
   {
      // start thinking
      sleep(get_random(1, MAX_THINK_EAT));

      // hungry
      pickup_forks(philosopher_number);

      // start eating
      sleep(get_random(1, MAX_THINK_EAT));

      // finish eating
      meals_eaten[philosopher_number] += 1;
      return_forks(philosopher_number);
   }

   // since we are done, exit
   pthread_exit(NULL);
}

void pickup_forks(int philosopher_number)
{
   int left_chopstick = get_left_chopstick_num(philosopher_number);
   int right_chopstick = get_right_chopstick_num(philosopher_number);

   // lock left chopstick
   pthread_mutex_lock(&mutexs[left_chopstick]);

   // conditionally release left chopstick until it is not available
   while (chopsticks[left_chopstick] != 1)
      pthread_cond_wait(&conditions[left_chopstick] , &mutexs[left_chopstick]);

   // lock right chopstick
   pthread_mutex_lock(&mutexs[right_chopstick]);

   // conditionally release right chopstick until it is not available
   while (chopsticks[right_chopstick] != 1)
      pthread_cond_wait(&conditions[right_chopstick] , &mutexs[right_chopstick]);

   
   // now both chopsticks are locked and available, make them unavailable
   chopsticks[left_chopstick] = 0;
   chopsticks[right_chopstick] = 0;
}

void return_forks(int philosopher_number)
{
   int left_chopstick = get_left_chopstick_num(philosopher_number);
   int right_chopstick = get_right_chopstick_num(philosopher_number);

   chopsticks[left_chopstick] = 1;
   pthread_mutex_unlock(&mutexs[left_chopstick]);
   chopsticks[right_chopstick] = 1;
   pthread_mutex_unlock(&mutexs[right_chopstick]);
}

int get_left_chopstick_num(int philosopher_number)
{
   return philosopher_number;
}

int get_right_chopstick_num(int philosopher_number)
{
   int chopstick_number = philosopher_number + 1;
   if (chopstick_number >= PHILOSOPHER_NUM)
   {
      chopstick_number = 0;
   }
   return chopstick_number;
}

double get_random(double min, double max)
{
   return min + ((double) rand() / RAND_MAX) * (max - min);
}