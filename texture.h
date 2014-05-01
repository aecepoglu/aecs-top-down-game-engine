/*
	texture handling definitions and functions
*/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL.h>

/* indexes for textures */
#define TEXTURE_TRN_NONE 		0
#define TEXTURE_TRN_WALL 		1
#define TEXTURES_COUNT_TERRAIN	2

#define TEXTURE_OBJ_PLAYER 		0
#define TEXTURE_OBJ_MONSTER 	1
#define TEXTURE_OBJ_APPLE    	2
#define TEXTURES_COUNT_OBJECT	3

struct GameTextures {
	SDL_Texture **trn;
	SDL_Texture ****obj;
};



/* Loads texture at given 'path' and returns it
*/
SDL_Texture* loadTexture( SDL_Renderer *ren, const char *path);

SDL_Texture*** loadTextureSheet( SDL_Renderer *ren, const char *path);

/* Draws given texture 'tex' inside the given rectangle
*/
void drawTexture( SDL_Renderer *ren, SDL_Texture *tex, int x, int y, int w, int h);

/* Loads all the textures
*/
struct GameTextures* loadAllTextures( SDL_Renderer *ren);

#endif //TEXTURE_H
