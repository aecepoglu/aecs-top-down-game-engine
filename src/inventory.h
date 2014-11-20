#ifndef INVENTORY_H
#define INVENTORY_H

#include "basic.h"
#include "object.h"

struct {
	int onAdd;
	int onRemove;
} inventoryCallbacks;

#define INVENTORY_SIZE 6

struct object *inventory[INVENTORY_SIZE];

void inventory_reset( bool deallocate);
bool inventory_add( struct object *obj, lua_State *lua);
struct object* inventory_remove( int index, lua_State *lua);

#endif /*INVENORY_H*/
