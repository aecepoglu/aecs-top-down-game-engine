#include "basic.h"

struct object* createObject( enum objType type, unsigned int x, unsigned int y) {
	struct object *o = (struct object*)malloc(sizeof(struct object));
	o->x = x;
	o->y = y;
	o->type = type;
	o->ai = 0;
	//TODO handle ai variable

	return o;
}

void writeObject( FILE *fp, struct object *obj) {
	fwrite( &obj->x,    sizeof(unsigned int), 1, fp);
	fwrite( &obj->y,    sizeof(unsigned int), 1, fp);
	fwrite( &obj->type, sizeof(enum objType), 1, fp);
}

struct object* readObject( FILE *fp) {
	struct object *obj = (struct object*)malloc(sizeof(struct object));
	fread( &obj->x,    sizeof(unsigned int), 1, fp);
	fread( &obj->y,    sizeof(unsigned int), 1, fp);
	fread( &obj->type, sizeof(unsigned int), 1, fp);
	return obj;
}
