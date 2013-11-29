#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>

struct Vector {
	int i, j ;
};

/* Sums two vectors. First vector has the result
*/
void vectorAdd( struct Vector*, struct Vector* );
/* Subtracts two vectors. First vector has the result
*/
void vectorSub( struct Vector*, struct Vector* );

void vectorReverse( struct Vector*);
/* bool clockwise: true if clockwise, false if counter-clockwise
*/
void vectorRotate( struct Vector*, bool);

struct Vector* readVector( FILE *fp);
void writeVector( FILE *fp, struct Vector*);

#define IS_VECTOR_IN_REGION( vec, x0, y0, x1, y1) ((vec.i > x0) && (vec.j > y0) && (vec.i < x1) && (vec.j < y1))

#endif //VECTOR_H
