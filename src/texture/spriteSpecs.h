#ifndef SPRITE_SPECS_H
#define SPRITE_SPECS_H

#include <stdio.h>
#include "../basic.h"

struct SpriteSpec {
	int id;
	char *path;

	int width;
	int height;
	int tallness;
};

struct SpriteSpecsList {
	struct SpriteSpec **array;
	int size;

	char *filePath;
};

#define SPRITE_SPECS_FILENAME "sprites.yz.cfg"


char* calculateSpriteSpecsFilePath( const char *mapPath);

struct SpriteSpecsList* readSpriteSpecsFile( const char *path);

void destroySpriteSpecs( struct SpriteSpecsList *SpriteSpecs);

void clearSpriteSpecs(struct SpriteSpecsList *s);
void loadSpriteSpecs(struct SpriteSpecsList *s, FILE *fp);

bool validateSpriteSpecs(const struct SpriteSpecsList *t);

#endif /*SPRITE_SPECS_H*/
