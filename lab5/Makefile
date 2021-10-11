TARGET = calcPI
CC = mpicc
CFLAGS = -Wall -c  -std=c99 -g -I/opt/intel/advisor/include
LFLAGS = -o $(TARGET) -lm -g -ldl

$(TARGET): $(TARGET).o integral.o
	$(CC) $(TARGET).o integral.o $(LFLAGS)

$(TARGET).o: $(TARGET).c integral.h
	$(CC) $(CFLAGS) $(TARGET).c

integral.o: integral.c integral.h
	$(CC) $(CFLAGS) integral.c

clean:
	rm -f $(TARGET) *.o *~ *#

