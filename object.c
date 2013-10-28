#include "object.h"
#include "aiTable.h"
#include "log.h"

struct object* createObject( enum objType type, unsigned int x, unsigned int y) {
	struct object *o = (struct object*)malloc(sizeof(struct object));
	o->x = x;
	o->y = y;
	o->type = type;
	o->dir = dir_up;
	o->ai = 0;

	return o;
}

void writeObject( FILE *fp, struct object *obj) {
	fwrite( &obj->x,    sizeof(unsigned int),   1, fp);
	fwrite( &obj->y,    sizeof(unsigned int),   1, fp);
	fwrite( &obj->type, sizeof(enum objType),   1, fp);
	fwrite( &obj->dir,  sizeof(enum direction), 1, fp);
	enum aiType tmp = obj->ai == 0 ? ai_none : obj->ai->type;
	fwrite( &tmp, 		sizeof(enum aiType),     1, fp);
}

struct object* readObject( FILE *fp) {
	struct object *obj = (struct object*)malloc(sizeof(struct object));
	fread( &obj->x,    sizeof(unsigned int),   1, fp);
	fread( &obj->y,    sizeof(unsigned int),   1, fp);
	fread( &obj->type, sizeof(enum objType),   1, fp);
	fread( &obj->dir,  sizeof(enum direction), 1, fp);
	enum aiType type;
	fread( &type,      sizeof(enum aiType),    1, fp);
	obj->ai = type != 0 ? AI_CREATE( type) : (struct AI*)0;
		
	return obj;
}
