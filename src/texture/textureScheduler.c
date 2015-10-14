#include "textureScheduler.h"
#include "../basic.h"
#include "../util/util.h"
#include "../util/log.h"

#include <stdio.h>
#include <string.h>

const char *TEXTURE_SCHEDULE_FILENAME = "textures.yz.sch";

char *getTextureSchedulePath( const char *file) {
	char *dirPath = getDirPath( file);

	char *result = combineFilePaths( dirPath, TEXTURE_SCHEDULE_FILENAME);

	free( dirPath);

	return result;
}

void unsetTexturePaths(struct TexturePaths *texturePaths, int from, int to) {
	int i;

	for( i=from; i<to; i++) {
		texturePaths->array[from] = NULL;
	}
}

struct TexturePaths *readTextureSchedule( const char *path) {
	FILE *fp = fopen( path, "r");
	if( fp == 0) {
		return NULL;
	}

	char *dirPath = getDirPath( path);

	char buf[BUFSIZ];
	char c;
	int i = 0;
	int lineNo = 1;

	struct TexturePaths *texturePaths = (struct TexturePaths*)malloc(sizeof(struct TexturePaths));

	texturePaths->size = 64;
	texturePaths->array = (char**)calloc(texturePaths->size, sizeof(char*));
	unsetTexturePaths( texturePaths, 0, 64);
	
	/* read textures first.
		this section is terminated by an empty line
	*/
	while( true) {
		c = fgetc( fp);
		buf[i] = c;
		i++;

		printf("'%c'\n", c);

		if (c == '\n' || c == EOF) {
			buf[i-1] = '\0';
			printf("%s\n", buf);

			char *splitPos = strchr(buf, ' ');

			if(splitPos != NULL) {
				*splitPos = '\0';

				int textureId = atoi( buf);
				char *filePath = combineFilePaths(dirPath, splitPos + 1);

				if( texturePaths->size < textureId) {
					texturePaths->size += 64;
					texturePaths->array = realloc( texturePaths->array, texturePaths->size * sizeof(char*));
					unsetTexturePaths( texturePaths, texturePaths->size - 64, texturePaths->size);
				}

				texturePaths->array[textureId] = filePath;
			}
			else if (i != 1) {
				fprintf(stderr, "Line '%s' isn't in form '<texture-id> <texture-path> at line%d\n", buf, lineNo);
				exit(1);
			}

			if (c == EOF) {
				break;
			}

			i = 0;
			lineNo ++;
		}

	}

	fclose( fp);
	free( dirPath);

	log1("Texture Paths:\n");
	for(i=0; i<texturePaths->size; i++) {
		if(texturePaths->array[i] != NULL) {
			log1("%d: %s\n", i, texturePaths->array[i]);
		}
	}
	

	return texturePaths;
}

void destroyTextureSchedule( struct TexturePaths *texturePaths) {
	int i;

	for( i=0; i<texturePaths->size; i++)
		if( texturePaths->array[i] != NULL)
			free(texturePaths->array[i]);

	free(texturePaths->array);
	free(texturePaths);
}
