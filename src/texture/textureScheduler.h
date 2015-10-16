#ifndef TEXTURE_SCHEDULER_H
#define TEXTURE_SCHEDULER_H

#include <stdio.h>

struct TexturePaths {
	char **array;
	int size;
	char *filePath;
};


char *getTextureSchedulePath( const char *mapPath);

struct TexturePaths *readTextureSchedule( const char *path);

void destroyTextureSchedule( struct TexturePaths *texturePaths);

void clearTexturePaths(struct TexturePaths *t);
void loadTexturePaths(struct TexturePaths *t, FILE *fp);

#endif /*TEXTURE_SCHEDULER_H*/
