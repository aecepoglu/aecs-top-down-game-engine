#include "texture.h"
#include <SDL_image.h>
#include <assert.h>
#include <stdlib.h>

#include "log.h"

SDL_Texture *loadTexture( SDL_Renderer *ren, const char *path){
    log1( "\tLoading %s\n", path);
	SDL_Surface *img = IMG_Load( path);
	assert( img);
	SDL_Texture *tex = SDL_CreateTextureFromSurface( ren, img);
	assert( tex);
	SDL_FreeSurface( img);

	return tex;
}

SDL_Texture ***loadTextureSheet( SDL_Renderer *ren, const char *path) {
    log1( "\tLoading sheet %s\n", path);
	SDL_Surface *img = IMG_Load( path);
	int edgeLen = img->w / 4;

	int numStates = img->h / edgeLen;
	log2( "\t\tWidth: %d, Height: %d. Num-states: %d, Edge-len:%d\n", img->w, img->h, numStates, edgeLen);
	assert(numStates >= 2);

	SDL_Texture ***result = calloc( numStates, sizeof(SDL_Texture **));



	SDL_Surface *blitSurf = SDL_CreateRGBSurface(0, edgeLen, edgeLen, img->format->BitsPerPixel, img->format->Rmask, img->format->Gmask, img->format->Bmask, img->format->Amask);
	SDL_Rect surfRect;
	surfRect.w = edgeLen;
	surfRect.h = edgeLen;

	//SDL_UnlockSurface(img);
	//SDL_UnlockSurface(&surf);

	int state, rot;
	for( state=0; state<numStates; state++) {
		surfRect.y = state * edgeLen;
		result[state] = calloc( 4, sizeof(SDL_Texture*));
		for( rot=0; rot<4; rot++) {
			surfRect.x = rot*edgeLen;
			
			if (SDL_BlitSurface( img, &surfRect, blitSurf, NULL) != 0)
				printf("SDL_BlitSurface failed: %s\n", SDL_GetError());

			result[state][rot] = SDL_CreateTextureFromSurface( ren, blitSurf);
		}
	}
	SDL_FreeSurface(img);
	SDL_FreeSurface(blitSurf);
	return result;
}

void drawTexture( SDL_Renderer *ren, SDL_Texture *tex, int x, int y, int w, int h) {
	SDL_Rect r = (SDL_Rect){x, y, w, h };
	SDL_RenderCopy( ren, tex, 0, &r);
}


struct GameTextures* loadAllTextures( SDL_Renderer *ren) {
	log0("Loading textures\n");
	
	struct GameTextures *result = malloc( sizeof(struct GameTextures));
	
	result->trn = calloc( TEXTURES_COUNT_TERRAIN, sizeof(SDL_Texture*));
	result->obj = calloc( TEXTURES_COUNT_OBJECT, sizeof(SDL_Texture***));

	result->trn[ TEXTURE_TRN_NONE 		] = loadTexture( ren, "res/ground.bmp");
	result->trn[ TEXTURE_TRN_WALL 		] = loadTexture( ren, "res/brick.png");


	result->obj[ TEXTURE_OBJ_PLAYER 	] = loadTextureSheet( ren, "res/player.png");
	result->obj[ TEXTURE_OBJ_MONSTER 	] = loadTextureSheet( ren, "res/monster.png");
	result->obj[ TEXTURE_OBJ_APPLE		] = loadTextureSheet( ren, "res/apple.png");

	//TODO duplicate sprite sheet and color it
	//SDL_SetTextureColorMod( result->objectTextures[ TEXTURE_OBJ_MONSTER], 255, 150, 255);

	return result;
}
