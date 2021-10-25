/* calcPI2.c calculates PI using POSIX threads.
 * Reduction pattern using pthreads
 *
 * Duncan Van Keulen for High Performance Computing Homework 6
 * At Calvin University, 22 October 2021
 */
#include "pthreadBarrier.h"

pthread_mutex_t lock;

// TODO: Works but breaks with odd number of processes...
/* Reduces local sums in an array
* @param: long double commArray[]: the array of values to reduce
* @param: unsigned long numThreads: the number of threads that are reducing
* @param: unsigned long id: the id of the thread "currently" running this function call
*/
long double pthreadReductionSum( long double commArray[], unsigned long numThreads, unsigned long id) {
    int myPartner = 0;
	for (int i = 2; i <= numThreads; i *= 2 ) {
		pthreadBarrier(numThreads);
        myPartner = (int)id + (i / 2);
        // add the value of your partner to your slot in the array
		if (id % i == 0) {
			commArray[id] += commArray[myPartner];
		}
	}
	return commArray[0];
}
