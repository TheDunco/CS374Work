/* calcPI2.c calculates PI using POSIX threads.
 * Reduction pattern using pthreads
 *
 * Duncan Van Keulen for High Performance Computing Homework 6
 * At Calvin University, 22 October 2021
 */
#include <pthread.h>               // pthreads

/*
 * @param localSum: the local sum of the thread to be added to the global sum var
 * @param pi: a pointer to the shared estimation value
 */
void pthreadReductionSum(long double localsum, volatile long double * pi) {
    
    
    
}