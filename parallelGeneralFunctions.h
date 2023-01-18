#ifndef PARALLEL_GENERAL_FUNCTIONS_H_
#define PARALLEL_GENERAL_FUNCTIONS_H_

#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include "structs.h"
#include "generalFunctions.h"

void bcastObjects(Image*** objects, int *numOfObs, int my_rank);

void bcastImage(Image** image, int my_rank);

void sendImage(Image* image, int dest);

void reciveImage(Image* image, int termination_tag, MPI_Status* status);

void sendMatch(Match* match);

void reciveMatch(Match* match, MPI_Status* status);

void sendPosition(Position* pos);

void recivePosition(Position* pos, int src, MPI_Status* status);

#endif
