#include <stdlib.h>
#include "array.h"

//TODO assuming uniform size. Handle objects of different size please
void arrayAdd( struct void *list, struct void *obj, unsigned int *count, unsigned int *size) {
	if ( *count == *size) {
		*size *= 2;
		list = realloc( list, *size);
	}

	list[ *count] = obj;
	*count ++;
}
