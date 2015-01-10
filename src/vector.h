#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include "basic.h"


struct Vector {
	int i, j ;
};

enum direction { dir_up, dir_right, dir_down, dir_left};

/* Sums two vectors. 3rd vector has the result
*/
void vectorAdd( struct Vector*, struct Vector* , struct Vector*);

/* Subtracts two vectors. 3rd vector has the result
*/
void vectorSub( struct Vector*, struct Vector*, struct Vector* );

void vectorClone( struct Vector *v, struct Vector*);

/* bool clockwise: true if clockwise, false if counter-clockwise
*/
void vectorRotate( struct Vector*, struct Vector*, bool);

bool vectorEquals( struct Vector*, struct Vector *);

struct Vector* readVector( FILE *fp);
void readToVector( FILE *fp, struct Vector*);
void writeVector( FILE *fp, struct Vector*);
enum direction vector_dirTan( int y, int x);
unsigned int vector_length( struct Vector *v);

extern struct Vector dirVectors[4];
extern uint8_t dirFlags[4];

#define DIR_ROTATE_RIGHT(x) ((x+dir_right)%4)
#define DIR_ROTATE_LEFT(x) ((x+dir_left)%4)
#define DIR_REVERSE(x) ((x+dir_down)%4)
#define IS_VECTOR_IN_REGION( vec, x0, y0, x1, y1) ((vec.i > x0) && (vec.j > y0) && (vec.i < x1) && (vec.j < y1))

#endif //VECTOR_H
