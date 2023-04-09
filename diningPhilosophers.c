// compile by running the make file
// or you can do: gcc -pthread -o dp diningPhilosophers.c
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
void* philosopher_run(void* philosopher_number_variable);
double get_random(double min, double max);
void pickup_chopsticks(int philosopher_number);
void wait_and_pickup_chopsticks(int philosopher_number);
void return_chopsticks(int philosopher_number);
int get_left_chopstick_num(int philosopher_number);
int get_right_chopstick_num(int philosopher_number);

// define global variable
int meals_eaten[PHILOSOPHER_NUM]; // array to keep track of meals eaten
int chopsticks[PHILOSOPHER_NUM]; // array to keep track of which chopstick is in use
pthread_cond_t waiting_for_chopsticks = PTHREAD_COND_INITIALIZER;
pthread_mutex_t chopsticks_mutex = PTHREAD_MUTEX_INITIALIZER;

// to ensure the program isn't allowing eating without chopsticks
int fail = 0;

// entry point of code
int main(int argc, char *argv[]) 
{
   // set initial conditions
   for(int i = 0; i < PHILOSOPHER_NUM; i++)
   {
      // make chopsticks available at the start
      chopsticks[i] = 1;

      // make meals eaten for each philospher 0
      meals_eaten[i] = 0; 
   }

   // an array to hold the thread objects
   pthread_t philosopherThreads[PHILOSOPHER_NUM];

   // create the threads
   for(int i = 0; i < PHILOSOPHER_NUM; i++)
   {
      // use a pointer to pass the num as void* param
      int *arg = malloc(sizeof(int)); 
      *arg = i;

      // create the thread
      pthread_create(&philosopherThreads[i], NULL, philosopher_run, arg);
   }

   // let threads run
   sleep(RUN_TIME);

   // cancel threads since our total runtime is over
   for(int i = 0; i < PHILOSOPHER_NUM; i++)
   {
      pthread_cancel(philosopherThreads[i]);
   }

   // print out the min, max, and average meals eaten
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

   if (fail)
   {
      printf("FAILURE\n");
   }

   return 0;
}

// function that is run on the threads
void* philosopher_run(void* philosopher_number_variable)
{
   // cast the parameter to the int we provided earlier
   int philosopher_number = *((int*)philosopher_number_variable);

   // make sure only 10 meals are eaten
   while(meals_eaten[philosopher_number] < MAX_MEALS)
   {
      //printf("%i thinking\n", philosopher_number);
      // start thinking
      sleep(get_random(1, MAX_THINK_EAT));

      //printf("%i hungry\n", philosopher_number);
      // hungry
      wait_and_pickup_chopsticks(philosopher_number);

      //printf("%i eating\n", philosopher_number);
      // start eating
      sleep(get_random(1, MAX_THINK_EAT));

      // finish eating
      meals_eaten[philosopher_number] += 1;

      return_chopsticks(philosopher_number);
   }

   // since we are done, exit
   pthread_exit(NULL);
}

void wait_and_pickup_chopsticks(int philosopher_number)
{
   int left_chopstick = get_left_chopstick_num(philosopher_number);
   int right_chopstick = get_right_chopstick_num(philosopher_number);

   // lock mutex so no one else can change value of chopsticks
   pthread_mutex_lock(&chopsticks_mutex);

   // wait until both available
   while(chopsticks[left_chopstick] != 1 || chopsticks[right_chopstick] != 1)
      pthread_cond_wait(&waiting_for_chopsticks, &chopsticks_mutex);

   // only 1 thread can be in this function at one point in time because of the mutex
   pickup_chopsticks(philosopher_number);

   // done changing chopsticks, release mutex
   pthread_mutex_unlock(&chopsticks_mutex);
}

void pickup_chopsticks(int philosopher_number)
{
   int left_chopstick = get_left_chopstick_num(philosopher_number);
   int right_chopstick = get_right_chopstick_num(philosopher_number);

   // if reached here and chopstick unavailable, its a failure
   if (chopsticks[left_chopstick] == 0 || chopsticks[right_chopstick] == 0)
   {
      fail = 1;
   }

   // now both chopsticks are locked and available, make them unavailable
   chopsticks[left_chopstick] = 0;
   chopsticks[right_chopstick] = 0;
}

void return_chopsticks(int philosopher_number)
{
   int left_chopstick = get_left_chopstick_num(philosopher_number);
   int right_chopstick = get_right_chopstick_num(philosopher_number);

   // lock mutex to set the avilability of chopsticks
   pthread_mutex_lock(&chopsticks_mutex);

   // if reached here and chopstick are available, its a failure
   if (chopsticks[left_chopstick] == 1 || chopsticks[right_chopstick] == 1)
   {
      fail = 1;
   }

   // make them available again
   chopsticks[left_chopstick] = 1;
   chopsticks[right_chopstick] = 1;

   // unlock the mutex and signal other threads
   pthread_mutex_unlock(&chopsticks_mutex);
   pthread_cond_signal(&waiting_for_chopsticks);
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