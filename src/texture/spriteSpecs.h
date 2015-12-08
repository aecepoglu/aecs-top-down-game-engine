#ifndef SPRITE_SPECS_H
#define SPRITE_SPECS_H

#include <stdio.h>
#include "../basic.h"

struct SpriteSpecs {
	char **array;
	int size;

	char *filePath;
};

#define SPRITE_SPECS_FILENAME "sprites.yz.cfg"


char* calculateSpriteSpecsFilePath( const char *mapPath);

struct SpriteSpecs* readSpriteSpecsFile( const char *path);

void destroySpriteSpecs( struct SpriteSpecs *SpriteSpecs);

void clearSpriteSpecs(struct SpriteSpecs *s);
void loadSpriteSpecs(struct SpriteSpecs *s, FILE *fp);

bool validateSpriteSpecs(const struct SpriteSpecs *t);

#endif /*SPRITE_SPECS_H*/
