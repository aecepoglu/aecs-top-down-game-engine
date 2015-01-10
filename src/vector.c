#include "vector.h"
#include <stdlib.h>
#include <math.h>


struct Vector dirVectors[] = {
	[dir_up] = { 0, -1},
	[dir_right] = { 1, 0},
	[dir_down] = {0, 1},
	[dir_left] = { -1, 0},
};
uint8_t dirFlags[] = {
	[dir_up] = 		0x00,
	[dir_right] = 	0x02,
	[dir_down] = 	0x04,
	[dir_left] = 	0x08,
};


void vectorAdd( struct Vector *v3, struct Vector *v1, struct Vector *v2) {
	v3->i = v1->i + v2->i;
	v3->j = v1->j + v2->j;
}
void vectorSub( struct Vector *v3, struct Vector *v1, struct Vector *v2) {
	v3->i = v1->i - v2->i;
	v3->j = v1->j - v2->j;
}
void vectorClone( struct Vector *v2, struct Vector *v1) {
	v2->i = v1->i;
	v2->j = v1->j;
}
void vectorRotate( struct Vector *v2, struct Vector *v, bool clockwise) {
	int tmp = v->i;

	int x = clockwise ? -1 : +1;
	v2->i = x * v->j;
	v2->j = -x * tmp;
}

bool vectorEquals( struct Vector* v1, struct Vector *v2) {
	return (v1->i == v2->i && v1->j == v2->j);
}

struct Vector *readVector( FILE *fp) {
	struct Vector *v = (struct Vector*)malloc( sizeof( struct Vector));
	//assume target is not null
	fread( &v->i, sizeof( int), 1, fp);
	fread( &v->j, sizeof( int), 1, fp);
	return v;
}
void readToVector( FILE *fp, struct Vector *v) {
	fread( &v->i, sizeof( int), 1, fp);
	fread( &v->j, sizeof( int), 1, fp);
}
void writeVector( FILE *fp, struct Vector *v) {
	fwrite( &v->i, sizeof( int), 1, fp);
	fwrite( &v->j, sizeof( int), 1, fp);
}

enum direction vector_dirTan( int y, int x) {
	bool f1 = (y-x) >= 0;
	bool f2 = (y+x) >= 0;

	enum direction result;
	if( f1)
		result = f2 ? dir_down : dir_left;
	else
		result = f2 ? dir_right : dir_up;
	
	return result;
}

unsigned int vector_length( struct Vector *v) {
	return abs( v->i) + abs( v->j);
}
