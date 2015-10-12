#include "guiHelpers.h"

#include <string.h>
#include <stdlib.h>

int parseText( const char *text) {
	if( strlen(text) == 0)
		return 0;
	else
		return atoi( text);
}

