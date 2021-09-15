PROG   = circuitSatisfiability
CC     = mpicc
CFLAGS = -Wall
LFLAGS = -o $(PROG)

$(PROG): $(PROG).c
	module load openmpi-2.0/gcc; \
	$(CC) $(CFLAGS) $(PROG).c $(LFLAGS)

clean:
	rm -f $(PROG) a.out *~ *# *.o *.out slurm*

