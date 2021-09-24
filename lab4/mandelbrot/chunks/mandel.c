/* Compute/draw mandelbrot set using MPI/MPE commands
 *
 * Written Winter, 1998, W. David Laverell.
 *
 * Refactored Winter 2002, Joel Adams. 
 * 
 * Modified Fall 2021, Duncan Van Keulen for HPC homework 4
 */

#include <stdio.h>
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

typedef struct {
   int ix;
   int iy;
   bool in;
} fractalPixel;


int main(int argc, char* argv[])
{
    const int  WINDOW_HEIGHT = 900; // 900
    const int  WINDOW_WIDTH  = 1200;
    const double SPACING     = 0.003; // 0.0025

   // instead of struct make matrix of bools
    fractalPixel pixelArray[WINDOW_WIDTH][WINDOW_HEIGHT] = {0};

    int        n            = 0,
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
    MPE_Open_graphics( &graph, MPI_COMM_WORLD, 
                         getDisplay(),
                         -1, -1,
                         WINDOW_WIDTH, WINDOW_HEIGHT, 0 );

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
            pixelArray[ix][iy] = new fractalPixel {
               ix = ix;
               iy = iy;
               in = false;
            }
            // MPE_Draw_point(graph, ix, iy, MPE_PINK);
         // point lies within the fractal
         } else {
            pixelArray[ix][iy] = new fractalPixel {
               ix = ix;
               iy = iy;
               in = true;
            }
            // MPE_Draw_point(graph, ix, iy, MPE_BLACK);
         }
       }
    }

    // pause until mouse-click so the program doesn't terminate
    if (id == 0) {
        printf("\nClick in the window to continue...\n");
        MPE_Get_mouse_press( graph, &ix, &iy, &button );
    }

    MPE_Close_graphics( &graph );
    MPI_Finalize();
    return 0;
}

