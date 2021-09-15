/*
* Duncan Van Keulen
* HPC Homework 2
* 15 September 2021
* Program sends a message in a loop between different processes
*   in parallel using MPI message sending and recieving and the 
*   master-worker structure.
* 
* To run: mpirun -np N -machinefile hosts ./hw2
* Note: Don't run this program with N <= 1
*/

#include <stdio.h>   // printf(), sprintf()
#include <mpi.h>     // MPI
#include <stdlib.h>  // malloc()
#include <string.h>  // strlen()

int main(int argc, char** argv) {
    int id = -1, numProcesses = -1, length = -1; 
    char hostName[MPI_MAX_PROCESSOR_NAME];
    MPI_Status status;
    double startTime = 0.0, totalTime = 0.0;


    // MPI init stuff
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Get_processor_name (hostName, &length);

    // character array stuff
    char * sendString = NULL;
    char * receivedString = NULL;
    int SIZE = 4 * numProcesses * sizeof(char);
    sendString = (char*) malloc( SIZE );
    receivedString = (char*) malloc( SIZE );

    startTime = MPI_Wtime();
    // if we are the master
    if(id == 0) {
        // make initial sendString of my rank, 0
        sprintf(sendString, "%d", id);
        // send message containing our rank to the next process
        //  msg, num values, type, dest
        MPI_Send(sendString, strlen(sendString) + 1, MPI_CHAR, ((id+1) % numProcesses), 1, MPI_COMM_WORLD);
        // recv message from the last host
        MPI_Recv(receivedString, SIZE, MPI_CHAR, (numProcesses - 1), 1, MPI_COMM_WORLD, &status);

        // calculate the time it took
        totalTime = MPI_Wtime() - startTime;
        // print recieved string and time.
        printf("%s\n", receivedString);
        printf("time: %f sec\n", totalTime);
    }
    // we are the worker
    else {
        // recieve from the process before us
        MPI_Recv(receivedString, SIZE, MPI_CHAR, (id-1), 1, MPI_COMM_WORLD, &status);
        // append your id to the "string"
        sprintf(sendString, "%s %d", receivedString, id);
        // send the message along to the next process
        MPI_Send(sendString, strlen(sendString) + 1, MPI_CHAR, ((id+1) % numProcesses), 1, MPI_COMM_WORLD);
    }
    // free up resources, finalize, and end normally
    free(sendString); free(receivedString);
    MPI_Finalize();
    return 0;
}