#include "parallelGeneralFunctions.h"


void bcastObjects(Image*** objects, int* numOfObs, int my_rank)
{
	// send number of objects to all processes
	MPI_Bcast(numOfObs, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if (my_rank != 0)
	{
		// allocate objects array
		*objects = (Image**) malloc(*numOfObs * sizeof(Image*));
		checkAllocation((void*) *objects);
	}
	
	// send all objects to all processes
	for (int i = 0; i < *numOfObs; i++)
	{	
		bcastImage((*objects) + i, my_rank);
	}
}

void bcastImage(Image** image, int my_rank)
{
	if (my_rank != 0)
	{
		// allocate image
		*image = (Image*) malloc(sizeof(Image));
		checkAllocation((void*) *image);
	}
	
	int* id = &((*image)->id);
	int* dim = &((*image)->dim);
	
	// send image id
	MPI_Bcast(id, 1, MPI_INT, 0, MPI_COMM_WORLD);
		
	// send image dim
	MPI_Bcast(dim, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	int numOfElements = (*dim) * (*dim);
		
	// allocate image pixels
	if (my_rank != 0)
	{
		(*image)->image = (int*) malloc(numOfElements * sizeof(int));
		checkAllocation((void*) ((*image)->image));
	}
		
	// send object's elements
	MPI_Bcast((*image)->image, numOfElements, MPI_INT, 0, MPI_COMM_WORLD);
}

void sendImage(Image* image, int dest)
{
	int* id = &(image->id);
	int* dim = &(image->dim);
	int* pixels = image->image;
	
	// send image id
	MPI_Send(id, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
		
	// send image dim
	MPI_Send(dim, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
	
	// send image pixels
	MPI_Send(pixels, (*dim) * (*dim), MPI_INT, dest, 0, MPI_COMM_WORLD);
}

void reciveImage(Image* image, int termination_tag, MPI_Status* status)
{
	int* id = &(image->id);
	int* dim = &(image->dim);
	
	// recive image id
	MPI_Recv(id, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, status); 
	
	if (status->MPI_TAG == termination_tag) // termination tag, stop
		return;
	
	// recive image dim
	MPI_Recv(dim, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, status); 
	
	// allocate image pixels
	int pixels = (*dim) * (*dim);
	image->image = (int*) realloc(image->image, pixels * sizeof(int));
	checkAllocation((void*) image->image);
	
	// recive image pixels
	MPI_Recv(image->image, pixels, MPI_INT, 0, 0, MPI_COMM_WORLD, status); 
}

void sendMatch(Match* match)
{
	// send image id
	MPI_Send(&(match->imageId), 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	
	// send object id
	MPI_Send(&(match->objectId), 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	
	// send position of the object in image
	sendPosition(match->pos);
}

void reciveMatch(Match* match, MPI_Status* status)
{
	// recive image id (from any source)
	MPI_Recv(&(match->imageId), 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, status); 
	
	// recive rest of the match only from the process who started sending it 
	int src = status->MPI_SOURCE;
	
	// recive object id
	MPI_Recv(&(match->objectId), 1, MPI_INT, src, 0, MPI_COMM_WORLD, status); 
	
	// recive position of the object in image
	recivePosition(match->pos, src, status);	
}

void sendPosition(Position* pos)
{
	// send i
	MPI_Send(&(pos->i), 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	
	// send j
	MPI_Send(&(pos->j), 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
}

void recivePosition(Position* pos, int src, MPI_Status* status)
{
	// recive i
	MPI_Recv(&(pos->i), 1, MPI_INT, src, 0, MPI_COMM_WORLD, status); 
	
	// recive j
	MPI_Recv(&(pos->j), 1, MPI_INT, src, 0, MPI_COMM_WORLD, status); 
}



