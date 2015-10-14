#ifndef TEXTURE_SCHEDULER_H
#define TEXTURE_SCHEDULER_H

struct TexturePaths {
	char **array;
	int size;
};


char *getTextureSchedulePath( const char *mapPath);

struct TexturePaths *readTextureSchedule( const char *path);

void destroyTextureSchedule( struct TexturePaths *texturePaths);

#endif /*TEXTURE_SCHEDULER_H*/
