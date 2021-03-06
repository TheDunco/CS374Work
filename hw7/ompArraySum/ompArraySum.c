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

void readArray(char * fileName, double ** a, int * n);
double sumArray(double * a, int numValues) ;

int main(int argc, char * argv[])
{
  int  howMany;
  double sum;
  double * a;
  double totalTime, ioTime, sumTime = 0;

  if (argc != 2) {
    fprintf(stderr, "\n*** Usage: arraySum <inputFile>\n\n");
    exit(1);
  }
  
  totalTime = omp_get_wtime();
  
  ioTime = omp_get_wtime();
  readArray(argv[1], &a, &howMany);
  ioTime = omp_get_wtime() - ioTime;
  
  sumTime = omp_get_wtime();
  sum = sumArray(a, howMany);
  sumTime = omp_get_wtime() - sumTime;
  
  totalTime = omp_get_wtime() - totalTime;
  
  printf("The sum of the values in the input file '%s' is %g\nSum time: %f\nIO Time: %f\nTotal time: %f\n",
           argv[1], sum, sumTime, ioTime, totalTime);

  free(a);

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

double sumArray(double * a, int numValues) {
  int i;
  double result = 0.0;

  #pragma omp parallel for reduction(+:result)
  for (i = 0; i < numValues; ++i) {
    result += a[i];
  }

  return result;
}

