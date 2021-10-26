/* calcPI2.c calculates PI using POSIX threads.
 * Reduction pattern using pthreads
 *
 * Duncan Van Keulen for High Performance Computing Homework 6
 * At Calvin University, 22 October 2021
 */
#include "pthreadBarrier.h"

pthread_mutex_t lock;

long double * commArray;  // the array to allow inter-thread communication
// TODO: Works but breaks with odd number of processes
/* Reduces local sums in an array
* @param: long double commArray[]: the array of values to reduce
* @param: unsigned long numThreads: the number of threads that are reducing
* @param: unsigned long id: the id of the thread "currently" running this function call
* @returns: a long double representing the reduction sum of the array
* Note: This algorithm should perform in O(log(n)) as long as there are enough cores to run the threads
*/
void pthreadReductionSum(long double localSum, unsigned long numThreads, unsigned long id, volatile long double * pi) {
	
	if (id == 0) {
		commArray = malloc(sizeof(long double) * numThreads);
	}
	pthreadBarrier(numThreads);
	commArray[id] = localSum;
	
	// multiply by 2 for correct number of iterations
	for (int i = 2; i < numThreads * 2; i *= 2) {
		pthreadBarrier((numThreads * 2 ) / i);
		
		// this checks if we are the "last" member of this stage in the reduction
		// otherwise we know our partner
		if (id % i == 0 && (id + i/2 < numThreads)) {
			commArray[id] += commArray[id + i/2];
		}
		else break;
	}
    barrierCleanup();
	if (id == 0) {
		*pi = commArray[0];
		free(commArray);
	}
}
