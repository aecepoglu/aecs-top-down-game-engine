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
