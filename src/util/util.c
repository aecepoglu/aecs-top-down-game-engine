#include "util.h"
#include "../basic.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *my_strcut( const char *from, const char *to) {
	/* result = strndup( file, slashPos - file);
		This is not available on non-gnu systems. I have to do it manually
	*/
	int len = to - from;
	
	char *result = calloc( len+1, sizeof(char));
	memcpy( result, from, len);

	result[len] = '\0';

	return result;
}

char* getDirPath( const char *file) {
	char *result;

	char *slashPos = strrchr( file, '/');
	if( slashPos == NULL) {
		result = strdup( ".");
	}
	else {
		result = my_strcut( file, slashPos);
	}

	return result;
}

char *combineFilePaths( const char *dir, const char *file) {
	int totalLength = strlen(dir) + strlen(file) + 2; /* one for '/', one for '\0' */

	EXIT_IF(totalLength >= MAX_FILEPATH_LENGTH, "file path \"%s/%s\" exceeds the maximum length: %d\n", dir, file, MAX_FILEPATH_LENGTH);

	char result[MAX_FILEPATH_LENGTH];
	sprintf(result, "%s/%s", dir, file);

	return strdup(result);
}
