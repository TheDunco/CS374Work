PROG    = ./mandel
LIB     = display

CC      = mpicc.mpich
CFLAGS  = -DMPE_GRAPHICS -c -Wall -ansi -pedantic -std=c99 
#LFLAGS  = -L/usr/X11R6/lib -lmpe -lX11 -lm 
LFLAGS  = -o $(PROG) -lmpe -lX11 -lm 

RUN	= mpirun.mpich
NP      = 1

$(PROG): $(PROG).o $(LIB).o 
	$(CC) $(PROG).o $(LIB).o $(LFLAGS)
	
$(PROG).o: $(PROG).c $(LIB).h
	$(CC) $(CFLAGS) $(PROG).c 

$(LIB).o: $(LIB).c $(LIB).h
	$(CC) $(CFLAGS) $(LIB).c 

# to run with 4 PEs, enter 'make run NP=4'
run: $(PROG)
	$(RUN) -np $(NP)  $(PROG)

clean:
	rm -f $(PROG) *.o *~ *# PI*

