/* Compute/draw mandelbrot set using MPI/MPE commands
 *
 * Written Winter, 1998, W. David Laverell.
 *
 * Refactored Winter 2002, Joel Adams. 
 * 
 * Modified Fall 2021, Duncan Van Keulen for HPC homework 4
 * Parallelized via the master-worker structure.
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
   *ans_x = x * x - y * y + c_real;
   *ans_y = 2 * x * y + c_imag;
}

/* compute the 'distance' between x and y.
 *
 * Receive: doubles x and y.
 * Return: x^2 + y^2.
 */
double distance(double x, double y)
{
   return x * x + y * y;
}

int main(int argc, char *argv[])
{
   const int WINDOW_HEIGHT = 900; // 900
   const int WINDOW_WIDTH = 1200;
   const int WINDOW_SIZE = WINDOW_HEIGHT * WINDOW_WIDTH;
   const double SPACING = 0.003; // 0.0025
   double startTime = 0.0, totalTime = 0.0;

   // instead of struct make matrix of bools
   bool *myRow;

   // bool **myWindowMatrix;
   // bool **finalWindowMatrix;

   int n = 0,
       ix = 0,
       iy = 0,
       button = 0,
       id = 0,
       numProcesses = 0;
   double x = 0.0,
          y = 0.0,
          c_real = 0.0,
          c_imag = 0.0,
          x_center = 1.16, // 1.16
       y_center = -0.1;    // 0.16

   MPE_XGraph graph;

   MPI_Init(&argc, &argv);

   MPI_Comm_rank(MPI_COMM_WORLD, &id);
   MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

   MPI_Status status;

   /*
    // Uncomment this block for interactive use
    printf("\nEnter spacing (.005): "); fflush(stdout);
    scanf("%lf",&spacing);
    printf("\nEnter coordinates of center point (0,0): "); fflush(stdout);
    scanf("%lf %lf", &x_center, &y_center);
    printf("\nSpacing=%lf, center=(%lf,%lf)\n",
            spacing, x_center, y_center);
*/

   if (numProcesses == 1)
   {
      MPE_Open_graphics(&graph, MPI_COMM_WORLD,
                        getDisplay(),
                        -1, -1,
                        WINDOW_WIDTH, WINDOW_HEIGHT, 0);

      startTime = MPI_Wtime();

      // Perform the entire computation myself using the default sequential approach
      for (ix = 0; ix < WINDOW_WIDTH; ix++)
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

            if (n < 50)
            {
               MPE_Draw_point(graph, ix, iy, MPE_PINK);
            }
            else
            {
               MPE_Draw_point(graph, ix, iy, MPE_BLACK);
            }
         }
      }

      totalTime = MPI_Wtime() - startTime;

      printf("%f", totalTime);

      printf("\nClick in the window to continue...\n");
      MPE_Get_mouse_press(graph, &ix, &iy, &button);
      MPE_Close_graphics(&graph);
   }
   else
   {
      myRow = (bool *)malloc(sizeof(bool *) * WINDOW_WIDTH);

      // Perform the calculation using the master-worker approach
      if (id == MASTER)
      {
         MPE_Open_graphics(&graph, MPI_COMM_WORLD,
                           getDisplay(),
                           -1, -1,
                           WINDOW_WIDTH, WINDOW_HEIGHT, 0);

         // start timing
         startTime = MPI_Wtime();

         // compute and display row 0
         for (ix = 0; ix < WINDOW_WIDTH; ix++)
         {
            c_real = (ix - 400) * SPACING - x_center;
            c_imag = (MASTER - 400) * SPACING - y_center;
            x = y = 0.0;
            n = 0;

            while (n < 50 && distance(x, y) < 4.0)
            {
               compute(x, y, c_real, c_imag, &x, &y);
               n++;
            }

            if (n < 50)
            {
               MPE_Draw_point(graph, ix, MASTER, MPE_PINK);
            }
            else
            {
               MPE_Draw_point(graph, ix, MASTER, MPE_BLACK);
            }
         }

         // variable to keep track of the next row that has to be computed
         int nextRow = numProcesses;
         int numDrawn = 1;

         while (numDrawn < WINDOW_HEIGHT)
         {
            // wait to receive a row from a worker
            MPI_Recv(myRow, WINDOW_WIDTH, MPI_C_BOOL, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            // the next row to compute will be the tag of the message received
            iy = status.MPI_TAG;

            // send that worker the next row to compute
            MPI_Send(&nextRow, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);

            if (nextRow < WINDOW_HEIGHT) { nextRow++; }

            // draw the row we recieved from the worker
            for (int i = 0; i < WINDOW_WIDTH; i++)
            {
               if (myRow[i])
               {
                  MPE_Draw_point(graph, i, iy, MPE_BLACK);
               }
               else
               {
                  MPE_Draw_point(graph, i, iy, MPE_PINK);
               }
            }

            numDrawn++;
         }

         // send termination message to all procs EXCEPT the master
         for (int i = 1; i < numProcesses; i++)
         {
            MPI_Send(&nextRow, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
         }

         totalTime = MPI_Wtime() - startTime;

         printf("%f", totalTime);

         printf("\nClick in the window to continue...\n");
         MPE_Get_mouse_press(graph, &ix, &iy, &button);
         MPE_Close_graphics(&graph);
      }
      // if we are a worker, calculate using the chunks of 1 (slices) approach
      else if (id != MASTER)
      {
         // until we get what row we need
         iy = id;

         while (true)
         {
            for (ix = 0; ix < WINDOW_WIDTH; ix++)
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
               if (n < 50)
               {
                  myRow[ix] = false;
                  // point lies within the WINDOW_HEIGHTal
               }
               else
               {
                  myRow[ix] = true;
               }
            }
            // send the row to the master
            MPI_Send(myRow, WINDOW_WIDTH, MPI_C_BOOL, 0, iy, MPI_COMM_WORLD);
            // recieve the next iy from the master
            MPI_Recv(&iy, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == 0)
            {
               break;
            }
         }
      }

      free(myRow);
   }

   MPI_Finalize();
   return 0;
}
