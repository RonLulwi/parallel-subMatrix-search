build:
	mpicxx -fopenmp -c main.c -o main.o
	mpicxx -fopenmp -c generalFunctions.c -o generalFunctions.o
	mpicxx -fopenmp -c parallelGeneralFunctions.c -o parallelGeneralFunctions.o
	mpicxx -fopenmp -c submatrixSearch.c -o submatrixSearch.o
	nvcc -I./inc -c cudaFunctions.cu -o cudaFunctions.o
	mpicxx -fopenmp -o mpiCudaOpemMP  main.o generalFunctions.o parallelGeneralFunctions.o submatrixSearch.o cudaFunctions.o  /usr/local/cuda/lib64/libcudart_static.a -ldl -lrt

clean:
	rm -f *.o ./mpiCudaOpemMP

run:
	mpiexec -np 3 ./mpiCudaOpemMP

runOn2:
	mpiexec -np 2 -machinefile  mf  -map-by  node  ./mpiCudaOpemMP

