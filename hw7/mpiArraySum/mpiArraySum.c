/* arraySum.c uses an array to sum the values in an input file,
 *  whose name is specified on the command-line.
 * Joel Adams, Fall 2005
 * for CS 374 (HPC) at Calvin College.
 * 
 * Modified for CS 374 (HPC) Homework 7 by Duncan Van Keulen
 * 5 November 2021
 * Added paralellization using MPI and timing.
*/

#include <stdio.h>      /* I/O stuff */
#include <stdlib.h>     /* calloc, etc. */
#include <mpi.h>

#define MASTER 0

void readArray(char * fileName, double ** a, int * n);
double sumArray(double * a, int numSent, int id, int numProcesses, double * scatterTime);

int main(int argc, char * argv[])
{
  int  howMany, remainder;
  double sum;
  double * a;
  double totalStartTime, totalEndTime = 0;
  double ioTime, scatterTime, sumTime = 0;
  int id, numProcesses, numSent = 0;
  
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
  
  totalStartTime = MPI_Wtime();

  if (argc != 2 && id == MASTER) {
    fprintf(stderr, "\n*** Usage: arraySum <inputFile>\n\n");
    exit(1);
  }
  
  if (id == MASTER) {
    ioTime = MPI_Wtime();
    readArray(argv[1], &a, &howMany);
    ioTime = MPI_Wtime() - ioTime;
  }

  MPI_Bcast(&howMany, 1, MPI_INT, 0, MPI_COMM_WORLD);
  
  // calculate how many array entries we are going to send to each process
  numSent = howMany / numProcesses;
  remainder = howMany % numProcesses;
  
  sumTime = MPI_Wtime();
  sum = sumArray(a, numSent, id, numProcesses, &scatterTime);
  
  if (id == MASTER) {
    // sum the remainder
    for (int i = howMany - remainder; i < howMany; i++) {
      sum += a[i];
    }
  }
  sumTime = MPI_Wtime() - sumTime;
  
  totalEndTime = MPI_Wtime() - totalStartTime;
  
  if (id == MASTER) {
    printf("The sum of the values in the input file '%s' is %g\nTotal time: %f seconds\nIO time: %f seconds \nScatter time: %f seconds\nSum time: %f seconds\n",
            argv[1], sum, totalEndTime, ioTime, scatterTime, sumTime);
    free(a);
  }

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

double sumArray(double * a, int numSent, int id, int numProcesses, double * scatterTime) {
  int i;
  double result, localResult = 0.0;
	double * aRcv;
  aRcv = (double*) malloc( numSent * sizeof(double) );

  *scatterTime = MPI_Wtime();
  
  MPI_Scatter(a, numSent, MPI_DOUBLE, aRcv, numSent, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  
  *scatterTime = MPI_Wtime() - *scatterTime;
  
  for (i = 0; i < numSent; ++i) {
    localResult += aRcv[i];
  }
  
  MPI_Reduce(&localResult, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  
  free(aRcv);

  return result;
}

