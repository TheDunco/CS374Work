/* arraySum.c uses an array to sum the values in an input file,
 *  whose name is specified on the command-line.
 * Joel Adams, Fall 2005
 * for CS 374 (HPC) at Calvin College.
 * 
 * Modified for CS 374 (HPC) Homework 7 by Duncan Van Keulen
 * 5 November 2021
 * Added paralellization and timing.
*/

#include <stdio.h>      /* I/O stuff */
#include <stdlib.h>     /* calloc, etc. */
#include <omp.h>
#include <mpi.h>

#define MASTER 0

void readArray(char * fileName, double ** a, int * n);
double sumArray(double * a, int numSent, int id, int numProcesses, double * aRcv);

int main(int argc, char * argv[])
{
  int  howMany;
  double sum;
  double * a;
  double * aRcv;
  double startTime, endTime = 0;
  int id, numProcesses, numSent = 0;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

  if (argc != 2 && id == MASTER) {
    fprintf(stderr, "\n*** Usage: arraySum <inputFile>\n\n");
    exit(1);
  }
  
  startTime = omp_get_wtime();
  
  readArray(argv[1], &a, &howMany);
  // calculate how many array entries we are going to send to each process
  numSent = howMany / numProcesses;
	aRcv = (double*) malloc( numSent * sizeof(double) );
    
  sum = sumArray(a, numSent, id, numProcesses, aRcv);
  
  endTime = omp_get_wtime() - startTime;
  
  if (id == MASTER) {
    printf("The sum of the values in the input file '%s' is %g\nCalculation took %f seconds to compute\n",
            argv[1], sum, endTime);
  }

  free(a);
  free(aRcv);
  MPI_Finalize();

  return 0;
}

/* readArray fills an array with values from a file.
 * Receive: fileName, a char*,
 *          a, the address of a pointer to an array,
 *          n, the address of an int.
 * PRE: fileName contains N, followed by N double values.
 * POST: a points to a dynamically allocated array
 *        containing the N values from fileName
 *        and n == N.
 */

void readArray(char * fileName, double ** a, int * n) {
  int count, howMany;
  double * tempA;
  FILE * fin;

  fin = fopen(fileName, "r");
  if (fin == NULL) {
    fprintf(stderr, "\n*** Unable to open input file '%s'\n\n",
                     fileName);
    exit(1);
  }

  fscanf(fin, "%d", &howMany);
  tempA = calloc(howMany, sizeof(double));
  if (tempA == NULL) {
    fprintf(stderr, "\n*** Unable to allocate %d-length array",
                     howMany);
    exit(1);
  }

  for (count = 0; count < howMany; count++)
   fscanf(fin, "%lf", &tempA[count]);

  fclose(fin);

  *n = howMany;
  *a = tempA;
}

/* sumArray sums the values in an array of doubles.
 * Receive: a, a pointer to the head of an array;
 *          numValues, the number of values in the array.
 * Return: the sum of the values in the array.
 */

double sumArray(double * a, int numSent, int id, int numProcesses, double * aRcv) {
  int i;
  double result = 0.0;

  MPI_Scatter(a, numSent, MPI_DOUBLE, aRcv, numSent, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  
  for (i = 0; i < numSent; ++i) {
    result += aRcv[i];
  }
  
  // TODO: changing from a to &result is giving seg faults on processes
  MPI_Reduce(&result, aRcv, numSent, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  return result;
}

