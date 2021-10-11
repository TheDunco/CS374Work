/* integral.h declares integral, a function to calculate
 *  definite integrals using the trapezoidal rule.
 *
 * Joel Adams, Fall 2013 for CS 374 at Calvin College.
 * 
 * Edited by Duncan Van Keulen at Calvin University for
 * High Performance Computing Homework 5
 * 11 October 2021
 */

long double integrateTrap(double xLo, double xHi,
                          unsigned long long numTrapezoids, int id, int numProcs, long double delta);

