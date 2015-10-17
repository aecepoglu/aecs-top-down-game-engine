#include "textureScheduler.h"
#include "../basic.h"
#include "../util/util.h"
#include "../util/log.h"
#include "../core/object.h"

#include <stdio.h>
#include <string.h>

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

void clearTexturePaths( struct TexturePaths *t) {
	int i;
	for (i=0; i<t->size; i++) {
		if (t->array[i] != NULL) {
			free(t->array[i]);
			t->array[i] = NULL;
		}
	}

	t->size = 0;
}

void loadTexturePaths( struct TexturePaths *t, FILE *fp) {
	char *dirPath = getDirPath( t->filePath);

	char buf[BUFSIZ];
	char c;
	int i = 0;
	int lineNo = 1;
	
	t->size = 64;
	t->array = (char**)calloc(t->size, sizeof(char*));
	unsetTexturePaths( t, 0, 64);
	
	/* read textures first.
		this section is terminated by an empty line
	*/
	while( true) {
		c = fgetc( fp);
		buf[i] = c;
		i++;

		if (c == '\n' || c == EOF) {
			buf[i-1] = '\0';

			char *splitPos = strchr(buf, ' ');

			if(splitPos != NULL) {
				*splitPos = '\0';

				int textureId = atoi( buf);
				char *filePath = combineFilePaths(dirPath, splitPos + 1);

				if( t->size < textureId) {
					t->size += 64;
					t->array = realloc( t->array, t->size * sizeof(char*));
					unsetTexturePaths( t, t->size - 64, t->size);
				}

				t->array[textureId] = filePath;
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

	free( dirPath);

	log1("Texture Paths:\n");
	for(i=0; i<t->size; i++) {
		if(t->array[i] != NULL) {
			log1("%d: %s\n", i, t->array[i]);
		}
	}
}

struct TexturePaths *readTextureSchedule( const char *path) {
	log1("loading texture-spec file \"%s\"\n", path);
	FILE *fp = fopen( path, "r");
	if( fp == 0) {
		log1("  file not found\n");
		return NULL;
	}


	struct TexturePaths *texturePaths = (struct TexturePaths*)malloc(sizeof(struct TexturePaths));

	texturePaths->filePath = strdup(path);

	loadTexturePaths( texturePaths, fp);

	fclose( fp);

	return texturePaths;
}

void destroyTextureSchedule( struct TexturePaths *t) {
	int i;

	for( i=0; i<t->size; i++)
		if( t->array[i] != NULL)
			free(t->array[i]);

	free(t->array);
	free(t->filePath);
	free(t);
}

bool validateTexturePaths(const struct TexturePaths *t) {
	if(t->size < 1) {
		fprintf(stderr, "The texture-spec file is empty\n");
		return false;
	}

	if(t->array[PLAYER_TEXTURE_ID] == NULL) {
		fprintf(stderr, "Texture-spec file must contain a texture with id %d for player\n", PLAYER_TEXTURE_ID);
		return false;
	}

	return true;
}
