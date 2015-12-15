/*
	texture handling definitions and functions
*/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL.h>
#include "../texture/spriteSpecs.h"

struct TextureSheet {
	SDL_Texture ***textures;
	int numStates;

	int spriteWidth;
	int spriteHeight;
};

struct GameTextures {
	SDL_Texture **trn;
	struct TextureSheet **obj;
	int objsCount;
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
struct TextureSheet* loadObjTextures( SDL_Renderer *ren, const struct SpriteSpec* spec);

/* Draws given texture 'tex' inside the given rectangle
*/
void drawTexture( SDL_Renderer *ren, SDL_Texture *tex, int x, int y, int w, int h);

/* Loads all the textures
*/
struct GameTextures* loadOrdinaryTextures( SDL_Renderer *ren);
void loadObjectTextures( SDL_Renderer *ren, struct GameTextures* textures,  const struct SpriteSpecsList *spriteSpecsList);

void texture_freeTextureSheet( struct TextureSheet *sheet);
void freeObjectTextures( struct GameTextures *textures);
void freeTextures( struct GameTextures* textures);

#endif //TEXTURE_H
