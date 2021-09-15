PROG    = parallelLoopChunks

CC      = mpicc
CFLAGS  = -DMPE_GRAPHICS -Wall -ansi -pedantic -std=c99 
LFLAGS  = -lmpe -lX11 -lm 

$(PROG): $(PROG).c
	$(CC) $(CFLAGS) $(PROG).c -o $(PROG) $(LFLAGS) 

clean:
	rm -f $(PROG) *.o *~ *# PI*

