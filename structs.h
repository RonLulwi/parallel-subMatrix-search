#ifndef STRUCST_H_
#define STRUCST_H_

typedef struct Image
{
	int id;
	int dim;
	int* image;
} Image;

typedef struct Position
{
	int i;
	int j;
} Position;

typedef struct Match
{
	int imageId;
	int objectId; // -1 means not found
	Position* pos; 
} Match;

#endif
