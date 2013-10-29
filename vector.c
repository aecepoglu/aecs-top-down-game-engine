#include "vector.h"

void vectorAdd( struct Vector *v1, struct Vector *v2) {
	v1->i += v2->i;
	v1->j += v2->j;
}
void vectorSub( struct Vector *v1, struct Vector *v2) {
	v1->i -= v2->i;
	v1->j -= v2->j;
}

void readVector( FILE *fp, struct Vector *v) {
	//assume target is not null
	fread( &v->i, sizeof( int), 1, fp);
	fread( &v->j, sizeof( int), 1, fp);
}
void writeVector( FILE *fp, struct Vector *v) {
	fwrite( &v->i, sizeof( int), 1, fp);
	fwrite( &v->j, sizeof( int), 1, fp);
}
