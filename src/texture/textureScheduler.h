#ifndef TEXTURE_SCHEDULER_H
#define TEXTURE_SCHEDULER_H

#include <stdio.h>
#include "../basic.h"

struct TexturePaths {
	char **array;
	int size;
	char *filePath;
};

#define TEXTURE_SCHEDULE_FILENAME "textures.yz.sch"


char *getTextureSchedulePath( const char *mapPath);

struct TexturePaths *readTextureSchedule( const char *path);

void destroyTextureSchedule( struct TexturePaths *texturePaths);

void clearTexturePaths(struct TexturePaths *t);
void loadTexturePaths(struct TexturePaths *t, FILE *fp);

bool validateTexturePaths(const struct TexturePaths *t);

#endif /*TEXTURE_SCHEDULER_H*/
