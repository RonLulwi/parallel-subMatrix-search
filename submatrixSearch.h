#ifndef SUBMATRIX_SEARCH_H_
#define SUBMATRIX_SEARCH_H_

#include "structs.h"
#include <stdlib.h>
#include "cudaFunctions.h"

void findAllObjectsInImage(Image* image, Image** objects, int numOfObs, double threshold, Match* match);

#endif

