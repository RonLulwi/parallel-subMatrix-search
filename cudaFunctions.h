#ifndef CUDA_FUNCTIONS_H_
#define CUDA_FUNCTIONS_H_

#include <stdlib.h>
#include "structs.h"

void sendImageToGPU(int** d_image, Image* image);

void freeImageOnGPU(int* d_image);

int computeOnGPU(int* d_image, int imageDim, Image* object, double threshold, Position* pos);

void allocateMemoryOnGPU(int** d_object, size_t objectSize, int** d_result, size_t resultsSize);

void copyDataFromHostToDevice(int* d_object, int* object, size_t objectSize);

int calculateResult(int* result, int imageDim, int objectDim, Position* pos);

#endif
