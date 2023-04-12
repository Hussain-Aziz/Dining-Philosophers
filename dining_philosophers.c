// compile by running the make file
// or you can do: gcc -pthread -o dp dining_philosophers.c
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

// define some constants
#define MAX_MEALS 10 
#define MAX_THINK_EAT 4.0

// some forward definitions
void parse_args(int* runtime, int argc, char* argv[]);
void* philosopher_run(void* philosopher_number_variable);
double get_random(double min, double max);
void pickup_chopsticks(int philosopher_number);
void return_chopsticks(int philosopher_number);
void compute_data();

// define global variable
int *meals_eaten; // array to keep track of meals eaten
int *chopsticks; // array to keep track of which chopstick is in use
int number_of_philosophers = 5; // globally defined for now
pthread_cond_t waiting_for_chopsticks = PTHREAD_COND_INITIALIZER;
pthread_mutex_t chopsticks_mutex = PTHREAD_MUTEX_INITIALIZER;

// entry point of code
int main(int argc, char *argv[]) 
{
   int runtime;
   parse_args(&runtime, argc, argv);

   // allocate memory for arrays
   meals_eaten = (int*)calloc(number_of_philosophers, sizeof(int));
   chopsticks = (int*)calloc(number_of_philosophers, sizeof(int));
   // an array to hold the thread objects
   pthread_t* philosopherThreads = (pthread_t*)calloc(number_of_philosophers, sizeof(pthread_t));

   // set initial conditions
   for(int i = 0; i < number_of_philosophers; i++)
   {
      // make chopsticks available at the start
      chopsticks[i] = 1;

      // make meals eaten for each philospher 0
      meals_eaten[i] = 0; 
   }

   // create the threads
   for(int i = 0; i < number_of_philosophers; i++)
   {
      // use a pointer to pass the num as void* param
      int *arg = malloc(sizeof(int)); 
      *arg = i;

      // create the thread
      pthread_create(&philosopherThreads[i], NULL, philosopher_run, arg);
   }

   // let threads run
   sleep(runtime);

   // cancel threads since our total runtime is over
   for(int i = 0; i < number_of_philosophers; i++)
   {
      pthread_cancel(philosopherThreads[i]);
   }

   // print out the min, max, and average meals eaten
   compute_data(runtime);

   // free the memory we allocated
   free(philosopherThreads);
   free(meals_eaten);
   free(chopsticks);

   return 0;
}

// function called at start to parse the command line arguments
// sets the runtime variable based on the given arguments
void parse_args(int* runtime, int argc, char* argv[])
{
   // verify correct number of arguments
   if (argc < 2)
   {
      printf("Too few arguments. Expecting 1 argument: runtime in seconds\n");
      exit(-1);
   }   
   else if (argc > 2)
   {
      printf("Too many arguments. Expecting 1 argument: runtime in seconds\n");
      exit(-1);
   }

   // set arguments
   *runtime = atoi(argv[1]);
}

// function that is run on the threads
void* philosopher_run(void* philosopher_number_variable)
{
   // cast the parameter to the int we provided earlier
   int philosopher_number = *((int*)philosopher_number_variable);
   
   // free the pointer to avoid memory leaks
   free(philosopher_number_variable);

   // make sure only 10 meals are eaten
   while(meals_eaten[philosopher_number] < MAX_MEALS)
   {
      // start thinking
      sleep(get_random(1, MAX_THINK_EAT));

      // hungry
      pickup_chopsticks(philosopher_number);

      // start eating
      sleep(get_random(1, MAX_THINK_EAT));

      // finish eating
      return_chopsticks(philosopher_number);
      meals_eaten[philosopher_number] += 1;
   }

   // since we are done, exit
   pthread_exit(NULL);
   return 0;
}

void pickup_chopsticks(int philosopher_number)
{
   int left_chopstick = philosopher_number;
   int right_chopstick = (philosopher_number + 1) % number_of_philosophers;

   // lock mutex so no one else can change value of chopsticks
   pthread_mutex_lock(&chopsticks_mutex);

   // wait until both available
   while(!(chopsticks[left_chopstick] == 1 && chopsticks[right_chopstick] == 1))
      pthread_cond_wait(&waiting_for_chopsticks, &chopsticks_mutex);

   // now both chopsticks are locked and available, make them unavailable
   chopsticks[left_chopstick] = 0;
   chopsticks[right_chopstick] = 0;

   // done changing chopsticks, release mutex
   pthread_mutex_unlock(&chopsticks_mutex);
}

// the function to put down the chopsticks
void return_chopsticks(int philosopher_number)
{
   int left_chopstick = philosopher_number;
   int right_chopstick = (philosopher_number + 1) % number_of_philosophers;

   // lock mutex to set the avilability of chopsticks
   pthread_mutex_lock(&chopsticks_mutex);

   // make them available again
   chopsticks[left_chopstick] = 1;
   chopsticks[right_chopstick] = 1;

   // unlock the mutex and signal other threads
   pthread_mutex_unlock(&chopsticks_mutex);
   pthread_cond_signal(&waiting_for_chopsticks);
}

double get_random(double min, double max)
{
   return min + ((double) rand() / RAND_MAX) * (max - min);
}

// a function to compute the data
void compute_data(int runtime)
{
   int min = __INT_MAX__, max = 0, totalMeals = 0;

   printf("Printing data for the %i seconds session:\n", runtime);

   for(int i = 0; i < number_of_philosophers; i++)
   {
      printf("Philosopher %i has eaten %i meals\n", i, meals_eaten[i]);
      totalMeals += meals_eaten[i];

      if(meals_eaten[i] < min) min = meals_eaten[i];
      if(meals_eaten[i] > max) max = meals_eaten[i];
   }

   // print the data
   printf("\nThe results for the %i seconds session:\n", runtime);
   printf("min: %i\nmax: %i\naverage: %f\n\n", min, max, (double)totalMeals/number_of_philosophers);
}