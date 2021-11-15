PROG   = circuitSatisfiability
CC     = mpicc
CFLAGS = -Wall
LFLAGS = -o $(PROG) -fopenmp

$(PROG): $(PROG).c
	$(CC) $(CFLAGS) $(PROG).c $(LFLAGS)

clean:
	rm -f $(PROG) a.out *~ *# *.o *.out slurm*

