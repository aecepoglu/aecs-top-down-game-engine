#define ARRAY_ADD( list, item, count, size, unitSize ) do { \
	if ( count == size) { \
		size *= 2; \
		list = realloc( list, size * unitSize); \
	} \
	list[ count] = item; \
	count ++; \
} while( 0)
