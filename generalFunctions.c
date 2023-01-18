#include "generalFunctions.h"


void readFile(double* threshold, int* numOfPics, Image*** images, int* numOfObs, Image*** objects)
{
	// open input file
	FILE* file = fopen(INPUT_FILE_NAME, "r");
	
	if (!file)
	{
		fprintf(stderr, "file can't be opened \n");
		exit(1);
	}
	
	// matching value
	fscanf(file, "%lf", threshold);
	
	// number of pictures
	fscanf(file, "%d", numOfPics);
	
	// allocate images array
	*images = (Image**) malloc(*numOfPics * sizeof(Image*));
	checkAllocation((void*) *images);
	
	// images
	for (int i = 0; i < *numOfPics; i++)
	{
		readImage(file, (*images) + i);
	}
	
	// number of objects
	fscanf(file, "%d", numOfObs);
	
	// allocate abjects array
	*objects = (Image**) malloc(*numOfObs * sizeof(Image*));
	checkAllocation((void*) *objects);
	
	// objects
	for (int i = 0; i < *numOfObs; i++)
	{
		readImage(file, (*objects) + i);
	}
	
	// close input file
	fclose(file);
}

void readImage(FILE* file, Image** image)
{
	// allocate image
	*image = (Image*) malloc(sizeof(Image));
	checkAllocation((void*) *image);
	
	// image id
	fscanf(file, "%d", &((*image)->id));
	
	// image dimension
	fscanf(file, "%d", &((*image)->dim));
	
	int numOfElements = ((*image)->dim) * ((*image)->dim);
	
	// allocate image elements
	(*image)->image = (int*) malloc(numOfElements * sizeof(int));
	checkAllocation((void*) ((*image)->image));
	
	// image elements
	for (int i = 0; i < numOfElements; i++)
	{
		fscanf(file, "%d", ((*image)->image) + i);
	}
}

void initMatches(Match*** matches, int numOfPics)
{
	// allocate matches array
	*matches = (Match**) malloc(numOfPics * sizeof(Match*));
	checkAllocation((void*) *matches);
	
	// matches 
	for (int i = 0; i < numOfPics; i++)
	{
		initMatch((*matches) + i);
	}
}

void initMatch(Match** match)
{
	// allocate match
	*match = (Match*) malloc(sizeof(Match));
	checkAllocation((void*) *match);
	
	// allocate position
	(*match)->pos = (Position*) malloc(sizeof(Position));
	checkAllocation((void*) ((*match)->pos));
}


void printResultsToFile(Match** matches, int numOfPics)
{
	// open output file
	FILE* file = fopen(OUTPUT_FILE_NAME, "w");
	
	if (!file)
	{
		fprintf(stderr, "file can't be opened \n");
		exit(1);
	}
	
	// print results
	for (int i = 0; i < numOfPics; i++)
	{
		int imageId = (matches[i])->imageId;
		int objectId = (matches[i])->objectId;
		Position* pos = (matches[i])->pos;
		
		if (objectId != -1) // object was found in image
		{
			fprintf(file, "Picture %d found Object %d in Position (%d,%d)\n", imageId,  objectId, pos->i, pos->j);
		}
		else // no object was found in image
		{
			fprintf(file, "Picture %d No Objects were found\n", imageId);
		}
	}
	
	// close output file
	fclose(file);	
}

void checkAllocation(void* ptr)
{
	if (!ptr)
	{
		fprintf(stderr, "Problem to allocate memory\n");
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}
}

void freeAll(Image** images, int numOfPics, Image** objects, int numOfObs, Match** matches, Image* myImage, Match* myMatch, int my_rank)
{
	// free objects 
	freeImages(objects, numOfObs);
	
	if (my_rank == 0) // master free all images and all matches
	{
		freeImages(images, numOfPics);
		freeMatches(matches, numOfPics);
	}
	else // slave free myImage and myMatch
	{
		freeImage(myImage);
		freeMatch(myMatch);
	}
}

void freeImages(Image** images, int numOfPics)
{
	for (int i = 0; i < numOfPics; i++)
	{
		freeImage(images[i]);
	}
	free(images);	
} 

void freeImage(Image* image)
{
	free(image->image);
	free(image);
}

void freeMatches(Match** matches, int numOfMatches)
{
	for (int i = 0; i < numOfMatches; i++)
	{
		freeMatch(matches[i]);
	}
	free(matches);
}

void freeMatch(Match* match)
{
	free(match->pos);
	free(match);
}


