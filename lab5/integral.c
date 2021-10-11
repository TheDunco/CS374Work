/* integral.c defines integrateTrap(), a function to approximate
 *  definite integrals using the trapezoidal rule. 
 *
 * Joel Adams, Fall 2013.
 * 
 * Parallelized using sclices by Duncan Van Keulen at Calvin University for 
 * High Performance Computing Homework 5
 * 11 October 2021
 */

#include "integral.h"
#include "advisor-annotate.h"



/* declaration of the function to be integrated,
 * which must be defined in the caller of integrate()
 * or a linking error will occur
 */
extern double f(double x); 

/* function to approximate an integral:
 * parameters: xLo, the left endpoint
 *             xHi, the right endpoint
 *             numTrapezoids, the number of trapezoids to use
 * return: the approximate integral of f(x) from x1 to x2.
 */
long double integrateTrap(double xLo, double xHi,
                          unsigned long long numTrapezoids,
                          int id, int numProcs, long double delta) {
   long double result = 0;
   unsigned long long i = 0;

   if (id == 0) {
    result = (f(xLo) + f(xHi)) * 0.5;
   }
   
  //  ANNOTATE_SITE_BEGIN("integrateTrap");
   for (i = id + 1; i < numTrapezoids; i += numProcs) {
    //  ANNOTATE_ITERATION_TASK("result")
     result += f(xLo + i*delta);
   }
   
  //  ANNOTATE_SITE_END("integrateTrap")

   return result;
} 

/* Note: The integral is approximately
 *   delta * (f(xLo) + f(x1)) / 2
 * + delta * (f(x1) + f(x2)) / 2
 * + delta * (f(x2) + f(x3)) / 2
 * ...
 * + delta * (f(x[n-2] + f(x[n-1]) / 2
 * + delta * (f(x[n-1]) + f(x[n])) / 2
 */

