#include "vector.h"
#include <stdlib.h>

void vectorAdd( struct Vector *v1, struct Vector *v2, struct Vector *v3) {
	v3->i = v1->i + v2->i;
	v3->j = v1->j + v2->j;
}
void vectorSub( struct Vector *v1, struct Vector *v2, struct Vector *v3) {
	v3->i = v1->i - v2->i;
	v3->j = v1->j - v2->j;
}
void vectorReverse( struct Vector *v1, struct Vector *v2) {
	v2->i = - v1->i;
	v2->j = - v1->j;
}
void vectorRotate( struct Vector *v, bool clockwise) {
	int tmp = v->i;
	if( clockwise) {
		v->i = - v->j;
		v->j = tmp;
	}
	else {
		v->i = v->j;
		v->j = -tmp;
	}
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

void vectorClone( struct Vector *v1, struct Vector *v2) {
	v2->i = v1->i;
	v2->j = v1->j;
}
