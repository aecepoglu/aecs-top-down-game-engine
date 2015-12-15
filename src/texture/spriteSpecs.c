#include "spriteSpecs.h"
#include "../basic.h"
#include "../util/util.h"
#include "../util/log.h"
#include "../core/object.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DELIMETER " ;:\t"

char *calculateSpriteSpecsFilePath( const char *file) {
	char *dirPath = getDirPath( file);

	char *result = combineFilePaths( dirPath, SPRITE_SPECS_FILENAME);

	free( dirPath);

	return result;
}

void unsetSpriteSpecs(struct SpriteSpecsList *spriteSpecs, int from, int to) {
	int i;

	for( i=from; i<to; i++) {
		spriteSpecs->array[from] = NULL;
	}
}

void clearSpriteSpecs( struct SpriteSpecsList *t) {
	int i;
	for (i=0; i<t->size; i++) {
		if (t->array[i] != NULL) {
			free(t->array[i]);
			t->array[i] = NULL;
		}
	}

	t->size = 0;
}

void loadSpriteSpecs( struct SpriteSpecsList *t, FILE *fp) {
	char *dirPath = getDirPath( t->filePath);
	char *string;
	char buf[BUFSIZ];

	t->size = 64;
	t->array = (struct SpriteSpec**)calloc(t->size, sizeof(struct SpriteSpec*));
	unsetSpriteSpecs( t, 0, 64);
	
	while (fgets(buf, BUFSIZ, fp)) {
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = '\0';

		struct SpriteSpec *sprite = (struct SpriteSpec*)malloc(sizeof(struct SpriteSpec));

		string = strtok(buf, DELIMETER);
		sprite->id = atoi(string);

		string = strtok(NULL, DELIMETER);
		sprite->path = combineFilePaths(dirPath, string);
		printf("sprite path: \"%s\"\n", sprite->path);

		string = strtok(NULL, DELIMETER);

		if (string) {
			sprite->width = atoi(string);

			string = strtok(NULL, DELIMETER);
			sprite->height = atoi(string);
		}
		else {
			sprite->width = 16;
			sprite->height = 16;
		}

		assert(sprite->width == sprite->height);

		if (sprite->id >= t->size) {
			t->size += 64;
			t->array = realloc(t->array, t->size * sizeof(char*));
			unsetSpriteSpecs(t, t->size - 64, t->size);
		}

		t->array[ sprite->id ] = sprite;
	}

	free( dirPath);

	log1("Sprite Specs:\n");
	int i = 0;
	for(i=0; i<t->size; i++) {
		if(t->array[i] != NULL) {
			log1("%d: %s\n", i, t->array[i]->path);
		}
	}
}

struct SpriteSpecsList *readSpriteSpecsFile( const char *path) {
	log1("loading texture-spec file \"%s\"\n", path);
	FILE *fp = fopen( path, "r");
	if( fp == 0) {
		log1("  file not found\n");
		return NULL;
	}


	struct SpriteSpecsList *spriteSpecs = (struct SpriteSpecsList*)malloc(sizeof(struct SpriteSpecsList));

	spriteSpecs->filePath = strdup(path);

	loadSpriteSpecs( spriteSpecs, fp);

	fclose( fp);

	return spriteSpecs;
}

void destroySpriteSpec(struct SpriteSpec *s) {
	free(s->path);
	free(s);
}

void destroySpriteSpecs( struct SpriteSpecsList *t) {
	int i;

	for( i=0; i<t->size; i++)
		if( t->array[i] != NULL)
			destroySpriteSpec(t->array[i]);

	free(t->array);
	free(t->filePath);
	free(t);
}

bool validateSpriteSpecs (const struct SpriteSpecsList *t) {
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
