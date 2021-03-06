/* calcPi.c calculates PI using the integral of the unit circle.
 * Since the area of the unit circle is PI, 
 *  PI = 4 * the area of a quarter of the unit circle 
 *    (i.e., its integral from 0.0 to 1.0)
 *
 * Joel Adams, Fall 2013 for CS 374 at Calvin College.
 * 
 * Parallelized using sclices by Duncan Van Keulen at Calvin University for 
 * High Performance Computing Homework 5
 * 11 October 2021
 * 
 */

#include "integral.h"   // integrate()
#include <stdio.h>      // printf(), etc.
#include <stdlib.h>     // exit()
#include <math.h>       // sqrt() 
#include <mpi.h>



/* function for unit circle (x^2 + y^2 = 1)
 * parameter: x, a double
 * return: sqrt(1 - x^2)
 */
double f(double x) {
   return sqrt(1.0 - x*x);
}

/* retrieve desired number of trapezoids from commandline arguments
 * parameters: argc: the argument count
 *             argv: the argument vector
 * return: the number of trapezoids to be used.
 */            
unsigned long long processCommandLine(int argc, char** argv) {
   if (argc == 1) {
       return 1;
   } else if (argc == 2) {
//       return atoi( argv[1] );
       return strtoull( argv[1], 0, 10 );
   } else {
       fprintf(stderr, "Usage: ./calcPI [numTrapezoids]");
       exit(1);
   }
}
 

int main(int argc, char** argv) {
   double startTime, endTime;
   int id, numProcs;
   long double finalPI = 0;
   long double approximatePI = 0;
   long double delta;
   const long double REFERENCE_PI = 3.141592653589793238462643383279L;
   unsigned long long numTrapezoids = processCommandLine(argc, argv); 
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &id);
   MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
   startTime = MPI_Wtime();

   // delta now calc'd here for MPI reduce
   delta = 1.0 / numTrapezoids;
   
   approximatePI = integrateTrap(0.0, 1.0, numTrapezoids, id, numProcs, delta);
   
   MPI_Reduce(&approximatePI, &finalPI, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
   
   if (id == 0) {
      // we factor the delta out of the operation and passed it in as a variable instead
      finalPI *= (4.0 * delta);
      endTime = MPI_Wtime() - startTime;

      printf("Using %llu trapezoids, the approximate vs actual values of PI are:\n%.30Lf\n%.30Lf\nProgram took %f seconds to run\n",
            numTrapezoids, finalPI, REFERENCE_PI, endTime);
   }
           
   MPI_Finalize();

   return 0;
}

