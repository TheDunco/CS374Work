/* Compute/draw mandelbrot set using MPI/MPE commands
 *
 * Written Winter, 1998, W. David Laverell.
 *
 * Refactored Winter 2002, Joel Adams. 
 * 
 * Modified Fall 2021, Duncan Van Keulen for HPC homework 4
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <sys/time.h>
#include <mpi.h>
#include <mpe.h>
#include "display.h"

#define MASTER 0

/* compute the Mandelbrot-set function for a given
 *  point in the complex plane.
 *
 * Receive: doubles x and y,
 *          complex c.
 * Modify: doubles ans_x and ans_y.
 * POST: ans_x and ans_y contain the results of the mandelbrot-set
 *        function for x, y, and c.
 */
void compute(double x, double y, double c_real, double c_imag,
              double *ans_x, double *ans_y)
{
        *ans_x = x*x - y*y + c_real;
        *ans_y = 2*x*y + c_imag;
}

/* compute the 'distance' between x and y.
 *
 * Receive: doubles x and y.
 * Return: x^2 + y^2.
 */
double distance(double x, double y)
{
   return x*x + y*y;
}

int main(int argc, char* argv[])
{
   const int  WINDOW_HEIGHT = 900; // 900
   const int  WINDOW_WIDTH  = 1200;
   const int WINDOW_SIZE = WINDOW_HEIGHT * WINDOW_WIDTH;
   const double SPACING     = 0.003; // 0.0025
   double startTime = 0.0, totalTime = 0.0;

   // instead of struct make matrix of bools
   bool * myWindow;
   bool * finalWindow;

   // bool **myWindowMatrix;
   // bool **finalWindowMatrix;

   int      n            = 0,
            ix           = 0,
            iy           = 0,
            button       = 0,
            id           = 0,
            numProcesses = 0,
            chunkSize    = 0,
            startPos     = 0,
            endPos       = 0;
   double     x            = 0.0,
            y            = 0.0,
            c_real       = 0.0,
            c_imag       = 0.0,
            x_center     = 1.16, // 1.16
            y_center     = -0.1; // 0.16

   MPE_XGraph graph;

   MPI_Init(&argc,&argv);

   MPI_Comm_rank(MPI_COMM_WORLD, &id);
   MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

   // calculate the size of the chunks we are using
   chunkSize = WINDOW_WIDTH / numProcesses;
   
   // calculate where we will start and where we wil lend
   startPos = id * chunkSize;
   endPos = (1 + id) * chunkSize;
/*
    // Uncomment this block for interactive use
    printf("\nEnter spacing (.005): "); fflush(stdout);
    scanf("%lf",&spacing);
    printf("\nEnter coordinates of center point (0,0): "); fflush(stdout);
    scanf("%lf %lf", &x_center, &y_center);
    printf("\nSpacing=%lf, center=(%lf,%lf)\n",
            spacing, x_center, y_center);
*/

   // allocate memory for the myWindow (size chunkSize * WINDOW_HEIGHT) and finalWindow
   myWindow = (bool *) malloc (sizeof(bool*) * chunkSize * WINDOW_HEIGHT);
   finalWindow = (bool *) malloc (sizeof(bool*) * WINDOW_SIZE);

   if (id == MASTER) {
      startTime = MPI_Wtime();
   }

   int xCount = 0;
   for (ix = startPos; ix < endPos; ix++)
   {
      for (iy = 0; iy < WINDOW_HEIGHT; iy++)
      {
         c_real = (ix - 400) * SPACING - x_center;
         c_imag = (iy - 400) * SPACING - y_center;
         x = y = 0.0;
         n = 0;

         while (n < 50 && distance(x, y) < 4.0)
         {
            compute(x, y, c_real, c_imag, &x, &y);
            n++;
         }

         // not in fractal
         if (n < 50) {
            myWindow[xCount * WINDOW_HEIGHT + iy ] = false;
         // point lies within the WINDOW_HEIGHTal
         } else {
            myWindow[xCount * WINDOW_HEIGHT + iy ] = true;
         }
      }
      xCount++;
   }

   MPI_Gather(myWindow, chunkSize * WINDOW_HEIGHT, MPI_C_BOOL, finalWindow, chunkSize * WINDOW_HEIGHT, MPI_C_BOOL, 0, MPI_COMM_WORLD);

   if (id == MASTER) {
   // only deal with grpahics if we are id 0
      MPE_Open_graphics( &graph, MPI_COMM_WORLD, 
                  getDisplay(),
                  -1, -1,
                  WINDOW_WIDTH, WINDOW_HEIGHT, 0 );

      for (ix = 0; ix < WINDOW_WIDTH; ix++)
      {
         for (iy = 0; iy < WINDOW_HEIGHT; iy++) {
            if(finalWindow[ix * WINDOW_HEIGHT + iy]) {
               MPE_Draw_point(graph, ix, iy, MPE_BLACK);
            } else {
               MPE_Draw_point(graph, ix, iy, MPE_PINK);
            }
         }
      }

      totalTime = MPI_Wtime() - startTime;

      printf("%f", totalTime);

      printf("\nClick in the window to continue...\n");
      MPE_Get_mouse_press( graph, &ix, &iy, &button );
      MPE_Close_graphics( &graph );
   }

   free(myWindow);
   free(finalWindow);

   MPI_Finalize();
   return 0;
}

