TARGET = vectorAdd
CC     = g++
CFLAGS = -Wall -ansi -pedantic -std=c99
LFLAGS = -o $(TARGET) -lgomp
NVCC = nvcc
# Use the next line in the Gold Lab (Maxwell architecture)
#CUDA_FLAGS = -gencode arch=compute_52,code=sm_52 
# Use the next line in the Maroon Lab (Pascal architecture)
CUDA_FLAGS = -gencode arch=compute_61,code=sm_61 

$(TARGET): $(TARGET).cu
	$(NVCC) $(CUDA_FLAGS) $(TARGET).cu $(LFLAGS)

clean: 
	rm -rf *o $(TARGET)
