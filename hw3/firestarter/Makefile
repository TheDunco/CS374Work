
# name of the binary
PROGRAM   = Fire
# source files
SRCS      = firestarter.c X-graph.c display.c
# object files from source files
OBJS      = $(SRCS:.c=.o)

# which compiler to use
CC        = mpicc
# flags for compilation and linking 
CFLAGS    = -I/usr/X11R6/include -Wall
LFLAGS    = -o $(PROGRAM) -L/usr/X11R6/lib -lX11 -lm

# valid file suffixes 
.SUFFIXES: .c .o .cpp

# command to build .o files from .c files
.c.o: 
	$(CC) -c $(CFLAGS) $<

# command to link object files into binary
$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) $(LFLAGS) 

# other dependencies (based on #includes)
X-graph.o: X-graph.h display.h
display.o: display.h
firestarter.o: X-graph.h

clean:
	/bin/rm -f $(OBJS) $(PROGRAM) *~ *#

