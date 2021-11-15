/* spmd-openmp.c - Openmp example program
 *
 * Code from:  http://www.cism.ucl.ac.be/XeonPhi.pdf
 * Transcribed and enhanced by: Chris Wieringa <cwieri39@calvin.edu> 2015-11-04
 *
 *  Usage: ./spmd-openmp [numThreads]
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

#define NAMELENGTH 256

int main (int argc, char *argv[])
{
  char hostname[NAMELENGTH];
  gethostname(hostname,NAMELENGTH);
  printf("Program launch host: %s\n", hostname);

  if (argc > 1) {
      omp_set_num_threads( atoi( argv[1] ) );
  }
  #pragma omp parallel 
  {
    int threadID = omp_get_thread_num();
    int numThreads = omp_get_num_threads();
    printf("Hello from thread %d of %d\n", threadID, numThreads);
    #pragma omp barrier
    #pragma omp single
    printf("There are %d cores on %s\n", omp_get_num_procs(), hostname);
  }
  return EXIT_SUCCESS;
}
