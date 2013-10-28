/*
	texture handling definitions and functions
*/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL.h>

/* indexes for textures */
#define TEXTURE_BAD				0
#define TEXTURE_TRN_NONE 		1
#define TEXTURE_TRN_WALL 		2
#define TEXTURE_OBJ_PLAYER 		3
#define TEXTURE_OBJ_MONSTER 	4

#define TEXTURE_MAP_TRN_TYPE( t)  (TEXTURE_TRN_NONE + t)
#define TEXTURE_MAP_OBJ_TYPE( t) (TEXTURE_OBJ_PLAYER + t)



/* Loads texture at given 'path' and returns it
*/
SDL_Texture *loadTexture( SDL_Renderer *ren, const char *path);

/* Draws given texture 'tex' inside the given rectangle
*/
void drawTexture( SDL_Renderer *ren, SDL_Texture *tex, int x, int y, int w, int h);

/* Loads all the textures
*/
SDL_Texture** loadTextures( SDL_Renderer *ren);

#endif //TEXTURE_H
