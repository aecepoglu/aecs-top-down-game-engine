#include "spriteSpecs.h"
#include "../basic.h"
#include "../util/util.h"
#include "../util/log.h"
#include "../core/object.h"
#include "../jsmn/jsmn.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

char* getJsmnStringValue(const char *string, const jsmntok_t *token) {
	EXIT_IF(token->type != JSMN_STRING, "Expected string instead of %.*s\n", token->end - token->start, string + token->start);
	return strndup(string + token->start, token->end - token->start);
}

int getJsmnIntValue(const char *string, const jsmntok_t *token) {
	EXIT_IF(token->type != JSMN_PRIMITIVE, "Expected number instead of %.*s\n", token->end - token->start, string + token->start);
	return strtol(string + token->start, NULL, 10);
}

int parseSpriteSpec(const char *string, const jsmntok_t *tokens, const char *pathPrefix, int start, struct SpriteSpec *target) {
	const char *KEYS[] = { "path", "width", "height", "tallness" };
	const int KEYS_SIZE = 4;

	jsmntok_t idToken = tokens[start];
	jsmntok_t spriteToken = tokens[start+1];

	EXIT_IF(false/* ||
		 string[idToken.start] < '0' ||
		 string[idToken.start] > '9'*/,
		"Expected '%.*s' to be a numeric sprite id\n", idToken.end - idToken.start, string + idToken.start
	);

	EXIT_IF(spriteToken.type != JSMN_OBJECT,
	        "Expected sprite definition '%.*s' to be a JSON object\n", spriteToken.end - spriteToken.start, string + spriteToken.start
	);

	int id = strtol(string + idToken.start, NULL, 10);
	
	target->id = id;
	target->path = NULL;
	target->width = 16;
	target->height = 16;
	target->tallness = 0;

	int i, tokenI;
	for (i = 0, tokenI = start + 2; i < spriteToken.size * 2; i += 2, tokenI += 2) {
		printf("* %.*s : %.*s\n", tokens[tokenI].end - tokens[tokenI].start, string + tokens[tokenI].start, tokens[tokenI+1].end - tokens[tokenI+1].start, string + tokens[tokenI+1].start);

		char *key = strndup(string + tokens[tokenI].start, tokens[tokenI].end - tokens[tokenI].start);
		const jsmntok_t *valueToken = &tokens[tokenI + 1];

		int j;
		for (j = 0; j < KEYS_SIZE; j ++)
			if (strcmp(key, KEYS[j]) == 0)
				break;

		free(key);

		switch (j) {
			case 0:
				{
					char *path = getJsmnStringValue(string, valueToken);
					target->path = combineFilePaths(pathPrefix, path);
					free(path);
					break;
				}
			case 1:
				target->width = getJsmnIntValue(string, valueToken);
				break;
			case 2:
				target->height = getJsmnIntValue(string, valueToken);
				break;
			case 3:
				target->tallness = getJsmnIntValue(string, valueToken);
				break;
			default:
				EXIT_IF(true, "unexpected key %s", key);
		};

	}

	EXIT_IF(target->path == NULL, "path is mandatory for sprite spefication\n");

	return spriteToken.size * 2 + 2;
}

void loadSpriteSpecs( struct SpriteSpecsList *t, FILE *fp) {
	char *dirPath = getDirPath( t->filePath);

	fseek(fp, 0, SEEK_END); //TODO check fseek return
	int dataSize = ftell(fp);
	char *data = (char*)calloc(dataSize, sizeof(char));

	rewind(fp);

	fread(data, sizeof(char), dataSize, fp);

	jsmn_parser parser;
	jsmn_init(&parser);

	int tokensCount = jsmn_parse(&parser, data, dataSize, NULL, 0);
	jsmn_init(&parser);

	jsmntok_t *tokens = calloc(tokensCount, sizeof(jsmntok_t));
	jsmn_parse(&parser, data, dataSize, tokens, tokensCount);

	t->size = 64;
	t->array = (struct SpriteSpec**)calloc(t->size, sizeof(struct SpriteSpec*));
	unsetSpriteSpecs( t, 0, 64);

	int tokenIndex = 1;
	while (tokenIndex < tokensCount) {
		struct SpriteSpec *sprite = (struct SpriteSpec*)malloc(sizeof(struct SpriteSpec));

		tokenIndex += parseSpriteSpec(data, tokens, dirPath, tokenIndex, sprite);

		if (sprite->id >= t->size) {
			t->size += 64;
			t->array = realloc(t->array, t->size * sizeof(char*));
			unsetSpriteSpecs(t, t->size - 64, t->size);
		}

		t->array[ sprite->id ] = sprite;
	}

	free(dirPath);
	free(data);
	free(tokens);

	log1("Sprite Specs:\n");
	int i = 0;
	for(i=0; i<t->size; i++) {
		if(t->array[i] != NULL) {
			log1("%d: path: %s\n", i, t->array[i]->path);
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
