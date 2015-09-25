/*
	texture handling definitions and functions
*/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL.h>

struct TextureSheet {
	SDL_Texture ***textures;
	int numStates;
};

struct GameTextures {
	SDL_Texture **trn;
	struct TextureSheet **obj;
	SDL_Texture **font;
	SDL_Texture *highlitObjIndicator,
				*unidentifiedObj,
				*fullHeart,
				*emptyHeart,
				*inventory;
};



/* Loads texture at given 'path' and returns it
*/
SDL_Texture* loadTexture( SDL_Renderer *ren, const char *path);

/* loads texture sheet at given path and returns it */
struct TextureSheet* loadObjTextures( SDL_Renderer *ren, const char *path);

/* Draws given texture 'tex' inside the given rectangle
*/
void drawTexture( SDL_Renderer *ren, SDL_Texture *tex, int x, int y, int w, int h);

/* Loads all the textures
*/
struct GameTextures* loadAllTextures( SDL_Renderer *ren);

void texture_freeTextureSheet( struct TextureSheet *sheet);
void freeTextures( struct GameTextures* textures);

#endif //TEXTURE_H
