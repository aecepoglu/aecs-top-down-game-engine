#include "vector.h"

void vectorAdd( struct Vector *v1, struct Vector *v2) {
	v1->i += v2->i;
	v1->j += v2->j;
}
void vectorSub( struct Vector *v1, struct Vector *v2) {
	v1->i -= v2->i;
	v1->j -= v2->j;
}
void vectorReverse( struct Vector *v) {
	v->x = - v->x;
	v->y = - v->y;
}
void vectorRotate( struct Vector *v, bool clockwise) {
	int tmp = v->x;
	if( clockwise) {
		v->x = - v->y;
		v->y = tmp;
	}
	else {
		v->x = v->y;
		v->y = -tmp;
	}
}

struct Vector *readVector( FILE *fp) {
	struct Vector *v = (struct Vector*)malloc( sizeof( struct Vector));
	//assume target is not null
	fread( &v->i, sizeof( int), 1, fp);
	fread( &v->j, sizeof( int), 1, fp);
	return v;
}
void writeVector( FILE *fp, struct Vector *v) {
	fwrite( &v->i, sizeof( int), 1, fp);
	fwrite( &v->j, sizeof( int), 1, fp);
}
