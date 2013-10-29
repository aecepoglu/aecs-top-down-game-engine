#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>

struct Vector {
	int i, j ;
};

void vectorAdd( struct Vector*, struct Vector* );
void vectorSub( struct Vector*, struct Vector* );

void readVector( FILE *fp, struct Vector*);
void writeVector( FILE *fp, struct Vector*);

#define IS_VECTOR_IN_REGION( vec, x0, y0, x1, y1) ((vec.i > x0) && (vec.j > y0) && (vec.i < x1) && (vec.j < y1))

#endif //VECTOR_H
