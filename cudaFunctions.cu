#include <cuda_runtime.h>
#include <helper_cuda.h>
#include "cudaFunctions.h"

void checkCuda(cudaError_t err);

void sendImageToGPU(int** d_image, Image* image)
{
	cudaError_t err;
	
	size_t imageSize = image->dim * image->dim * sizeof(int);
	
    	// allocate image memory on GPU
    	err = cudaMalloc((void **)d_image, imageSize);
    	checkCuda(err);
    	
    	// copy image from host to device
    	err = cudaMemcpy(*d_image, image->image, imageSize, cudaMemcpyHostToDevice);
    	checkCuda(err);
}

void freeImageOnGPU(int* d_image)
{
	cudaError_t err = cudaFree(d_image);
	checkCuda(err);
}
	


__global__ void kernel(int* image, int imageDim, int* object, int objectDim, double threshold, int* results)
{
	double matching = 0;
	double diff;
	int p;
	int o;
	int k = 0;
	int edge = imageDim - objectDim + 1;
	
	// each thread will check if the object is in a specific location in the image (calculate the matching value) 
 	
 	int myPos = blockDim.x * blockIdx.x + threadIdx.x;
	int row = myPos / imageDim;
 	int col = myPos % imageDim;
	
	if (row < edge && col < edge) // check if my position can consist the object 
	{
		int* myImage = image + myPos;
		for (int i = 0; i < objectDim && matching <= threshold; i++)
		{
			for (int j = 0; j < objectDim && matching <= threshold; j++, k++)
			{
				p = *(myImage + imageDim *  i + j);
				o = (object)[k];
				diff = abs(p-o) / (double) p;
				matching = matching + diff;
			}
		}
		
	 	results[myPos] = (matching <= threshold);
 	}
}


int computeOnGPU(int* d_image, int imageDim, Image* object, double threshold, Position* pos)
{
	// Error code to check return values for CUDA calls
    	cudaError_t err = cudaSuccess;
    	
    	int objectDim = object->dim;
    	
    	int imagePixels = imageDim * imageDim;
    	int objectPixels = objectDim * objectDim;
    	
    	int* d_object;
    	int* d_result;
    	
    	size_t imageSize = imagePixels * sizeof(int);
    	size_t objectSize = objectPixels * sizeof(int);
    	
	// Allocate memory on GPU 
	allocateMemoryOnGPU(&d_object, objectSize, &d_result, imageSize);
   	
    	// Copy object from host to GPU memory
    	copyDataFromHostToDevice(d_object, object->image, objectSize);
    	
    	// allocate results array on host
   	int* result = (int*) malloc(imageSize * sizeof(int));
   	if (!result)
   	{
   		fprintf(stderr, "Problem to allocate memory\n");
        	exit(EXIT_FAILURE);
   	}
   	
   	
   	// lauch the kernel for searching object in image
   	int threadsPerBlock = 256;
    	int blocksPerGrid = (imagePixels / threadsPerBlock) + ((imagePixels % threadsPerBlock) != 0);
    	
    	kernel<<<blocksPerGrid, threadsPerBlock>>>(d_image, imageDim, d_object, objectDim, threshold, d_result);
    	checkCuda(cudaGetLastError());

    	// Copy the  result from GPU to the host memory.
    	err = cudaMemcpy(result, d_result, imageSize, cudaMemcpyDeviceToHost);
    	checkCuda(err);
    	
    	// Calculate the result
    	int found = calculateResult(result, imageDim, objectDim, pos);
    		
    	// Free allocated memory 
    	if (cudaFree(d_object) != cudaSuccess || cudaFree(d_result) != cudaSuccess) 
    	{
    		fprintf(stderr,"%s\n", cudaGetErrorString(err));
     	   	exit(EXIT_FAILURE);
    	}
    	
    	free(result);
    	
    	return found;	
}

void allocateMemoryOnGPU(int** d_object, size_t objectSize, int** d_result, size_t resultsSize)
{
	cudaError_t err;
    	
    	// object
    	err = cudaMalloc((void **)d_object, objectSize);
        checkCuda(err);
    	
    	// result array
    	err = cudaMalloc((void **)d_result, resultsSize);
        checkCuda(err);
}

void copyDataFromHostToDevice(int* d_object, int* object, size_t objectSize)
{
	cudaError_t err;
    	
   	// object
   	err = cudaMemcpy(d_object, object, objectSize, cudaMemcpyHostToDevice);
    	checkCuda(err);
}

int calculateResult(int* result, int imageDim, int objectDim, Position* pos)
{
	int edge = imageDim - objectDim + 1;
	
    	for (int i = 0; i < edge; i++)
    	{
    		for(int j =0; j < edge; j++)
    		{
    			if( *(result + (imageDim * i) + j) )
    			{
				pos->i = i;
				pos->j =j;
				return 1;
			}	
    		}
    	}
    	
    	return 0;
}

void checkCuda(cudaError_t err)
{
	if (err != cudaSuccess) 
        {
        	fprintf(stderr, "%s\n", cudaGetErrorString(err));
        	exit(EXIT_FAILURE);
    	}
}


