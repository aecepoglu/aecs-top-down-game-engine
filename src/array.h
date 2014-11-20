#define ARRAY_ADD( list, item, count, size, unitSize ) do { \
	if ( count == size) { \
		size *= 2; \
		list = realloc( list, size * unitSize); \
	} \
	list[ count] = item; \
	count ++; \
} while( 0)

#define ARRAY_REMOVE( list, item, count) do {\
	int i;\
	for( i=0; i<count; i++) {\
		if( list[i] == item) {\
			int j;\
			for( j=i+1; j<count; i++, j++) {\
				list[i] = list[j];\
			}\
		}\
	}\
} while( 0)
