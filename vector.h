#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include "basic.h"


struct Vector {
	int i, j ;
};

/* Sums two vectors. 3rd vector has the result
*/
void vectorAdd( struct Vector*, struct Vector* , struct Vector*);

/* Subtracts two vectors. 3rd vector has the result
*/
void vectorSub( struct Vector*, struct Vector*, struct Vector* );

void vectorReverse( struct Vector*, struct Vector*);

void vectorClone( struct Vector *v, struct Vector*);

/* bool clockwise: true if clockwise, false if counter-clockwise
*/
void vectorRotate( struct Vector*, bool);

struct Vector* readVector( FILE *fp);
void readToVector( FILE *fp, struct Vector*);
void writeVector( FILE *fp, struct Vector*);

enum direction { dir_up, dir_right, dir_down, dir_left};
extern struct Vector dirVectors[4];

#define IS_VECTOR_IN_REGION( vec, x0, y0, x1, y1) ((vec.i > x0) && (vec.j > y0) && (vec.i < x1) && (vec.j < y1))

#endif //VECTOR_H
