#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"
#include "generalFunctions.h"
#include "parallelGeneralFunctions.h"
#include "submatrixSearch.h"


/*

The master sends all objects to all processes. Then the master sends each slave one image to work on. After the slave finishes with his image, it sends the result to the master, and if there are more image left to process, the master sends anouther one to the same slave (dynamic images allocation).  

Each slave searches for all of the objects in his image in parallel using openmp (each thread will search for a different object in the image). The search for one object in image is used with cuda - each thread calculate the matching value of one position in the image.    

*/

int main(int argc, char* argv[])
{
	int size; 
	int  my_rank; 
	MPI_Status status;
	int termination_tag = 1;
	
	double threshold;
	int numOfPics; 
	Image** images; 
	int numOfObs; 
	Image** objects;
	Match** matches;
	
	Image* myImage;
	Match* myMatch;
	
	clock_t begin, end;
	
	// init MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	if (my_rank == 0)
	{
		// start timer
		begin = clock();
		
		// read input file
		readFile(&threshold, &numOfPics, &images, &numOfObs, &objects);
		
		// init matches array
		initMatches(&matches, numOfPics);
	}
	else
	{
		// allocate my image 
		myImage = (Image*) malloc(sizeof(Image));
		checkAllocation((void*) myImage);
		myImage->image = NULL;
		
		// init my match
		initMatch(&myMatch);
	}
	
	// send threshold to all processes
	MPI_Bcast(&threshold, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	// send number of images to all processes
	MPI_Bcast(&numOfPics, 1, MPI_INT, 0, MPI_COMM_WORLD);	
	
	// send all objects to all processes
	bcastObjects(&objects, &numOfObs, my_rank);
	
	// master
	if (my_rank == 0)
	{
		int send = 0;
		int recive = 0;
		
		// send each slave one image
		for (int i = 1; i < size && send < numOfPics; i++)
		{
			sendImage(images[send], i);
			send++;
		}
		
		while (recive < numOfPics)
		{
			// recive next result
			reciveMatch(matches[recive], &status);
			recive++;
			int sender = status.MPI_SOURCE;
			if (send < numOfPics) // send next image to available slave
			{
				sendImage(images[send], sender);
				send++;
			}
			else // no images left, send termination tag 
			{
				int tmp;
				MPI_Send(&tmp, 1, MPI_INT, sender, termination_tag, MPI_COMM_WORLD);	
			}
			
		}
		
		// print results to file
		printResultsToFile(matches, numOfPics);
	
		// stop timer
		end = clock();
		
		double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		printf("Time = %lf seconds\n", time_spent);
	} 
	else // slave
	{
		if (my_rank <= numOfPics) // recived at least one image (for case when number of processes is greater then number of images)
		{
			while (1)
			{
				// recive next image
				reciveImage(myImage, termination_tag, &status);
				
				if (status.MPI_TAG == termination_tag) // termination tag, stop
					break;
				else // search objects in the image
				{
					findAllObjectsInImage(myImage, objects, numOfObs, threshold, myMatch);
					
					// send result to master
					sendMatch(myMatch);
				}	
			} 
		}
	}
	
	
	// free allocated memory	
	freeAll(images, numOfPics, objects, numOfObs, matches, myImage, myMatch, my_rank);
	
	
	MPI_Finalize();
	
	return 0;
}
