#include "object.h"
#include "log.h"
#include "aiTable.h"
#include <stdlib.h>

struct object* createObject( enum objType type, unsigned int x, unsigned int y) {
	struct object *o = (struct object*)malloc(sizeof(struct object));
	o->pos.i = x;
	o->pos.j = y;
	o->type = type;
	o->dir = dir_right;
	o->ai = 0;

	return o;
}

void writeObject( FILE *fp, struct object *obj) {
	writeVector( fp, &obj->pos);
	fwrite( &obj->type, sizeof(enum objType),   1, fp);
	fwrite( &obj->dir,  sizeof(enum direction), 1, fp);
	enum aiType tmp = obj->ai == 0 ? ai_none : obj->ai->type;
	fwrite( &tmp, 		sizeof(enum aiType),     1, fp);
}

struct object* readObject( FILE *fp) {
	struct object *obj = (struct object*)malloc(sizeof(struct object));
	readToVector( fp, &obj->pos);
	fread( &obj->type, sizeof(enum objType),   1, fp);
	fread( &obj->dir,  sizeof(enum direction), 1, fp);
	enum aiType type;
	fread( &type,      sizeof(enum aiType),    1, fp);
	obj->ai = type != 0 ? AI_CREATE( type) : 0;

	return obj;
}

/*
	Some functions objects use to interract with each other
*/

void use( struct object *obj, struct object *obj2) {
	/* Not used yet. Might even need to be implemented inside the object */
}

void hit( struct object *obj1, struct object *obj2) {
	obj2->health -= 1;
}

void swallow( struct object *obj1, struct object *obj2) {
	/* People can eat each other while alive.
		Change this to obj1->health += obj2->maxHealth/2
		Also only eat dead objects
	*/
	obj1->health += obj2->health;
}
