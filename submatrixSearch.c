#include "submatrixSearch.h"

void findAllObjectsInImage(Image* image, Image** objects, int numOfObs, double threshold, Match* match)
{
	match->imageId = image->id;
	match->objectId = -1;
	int found = 0;
	int my_result = 0;
	Position my_pos = { -1, -1 };
	
	// each thread will search for a different object in the image using CUDA
	
	// send image to GPU
	int* d_image;
	sendImageToGPU(&d_image, image);
	
	# pragma omp parallel for private(my_result, my_pos)
	for (int i = 0; i < numOfObs; i++)
	{
		if (!found) // check if one of the other threads has already found an object
		{	
			my_result = computeOnGPU(d_image, image->dim, objects[i], threshold, &my_pos);
			#pragma omp critical
			{
				if (my_result && !found)
				{
					match->objectId = (objects[i])->id;
					(match->pos)->i = my_pos.i;
					(match->pos)->j = my_pos.j;
					found = 1;
				}
			}
		}
	}
	
	// free image memory on GPU
	freeImageOnGPU(d_image);
}

