# CS 47200 Project: The Dining Philosophers Problem  

## 1. Introduction 
This project implements a solution to the Dining Philosophers Problem using Pthreads, POSIX mutex locks, and condition variables.  

## 2. Project Description 
The main function will create PHILOSOPHER_NUM (5) threads for philosophers and will sleep for a 
period of time (run_time), and upon awakening, will terminate all threads and print out the numbers of  meals each philosopher eats and calculate the minimum, the maximum, and the average numbers of 
meals eaten among philosophers. A philosopher thread exits when the MAX_MEALS (10) meals have 
been completed. Use sleep function for random one to MAX_THINK_EAT_SEC (4) double value seconds 
thinking and eating time. The main function will be passed one parameter run_time on the command 
line. The program may be named dinning_philos.c. To simulate the monitor solution, a mutex lock must be used to lock the state and condition variables at the start and unlock at the end in pickup and putdown functions. The condition can be state[i] == EATING for ith philosopher.

## Submission
Submit a report with the minimum, maximum, and average meals eaten by a philosopher in 30, 40, 50, 60, 70, and 80 seconds of run_time.