#include <SDL_image.h>
#include <assert.h>
#include <stdlib.h>

#include "texture.h"
#include "object.h"
#include "log.h"

#define SPRITE_TILE_LEN 16

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

	int numRotations = img->w / SPRITE_TILE_LEN;

	int numStates = img->h / SPRITE_TILE_LEN;
	log0( "\t\tWidth: %d, Height: %d. Num-states: %d, Num-rotations:%d\n", img->w, img->h, numStates, numRotations);
	assert(numStates >= 2);

	SDL_Texture ***result = calloc( numStates, sizeof(SDL_Texture **));



	SDL_Surface *blitSurf = SDL_CreateRGBSurface(0, SPRITE_TILE_LEN, SPRITE_TILE_LEN, img->format->BitsPerPixel, img->format->Rmask, img->format->Gmask, img->format->Bmask, img->format->Amask);
	SDL_Rect surfRect;
	surfRect.w = SPRITE_TILE_LEN;
	surfRect.h = SPRITE_TILE_LEN;

	//SDL_UnlockSurface(img);
	//SDL_UnlockSurface(&surf);

	int state, rot;
	for( state=0; state<numStates; state++) {
		surfRect.y = state * SPRITE_TILE_LEN;
		result[state] = calloc( 4, sizeof(SDL_Texture*));
		for( rot=0; rot<4; rot++) {
			surfRect.x = (rot % numRotations)*SPRITE_TILE_LEN;
	
			SDL_FillRect( blitSurf, NULL, 0);
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

	result->trn[ TEXTURE_TRN_NONE 		] = loadTexture( ren, "res/ground.png");
	result->trn[ TEXTURE_TRN_WALL 		] = loadTexture( ren, "res/brick.png");

	result->obj = calloc( go_NUM_ITEMS, sizeof(SDL_Texture***));

	result->obj[ go_player] = loadTextureSheet( ren, "res/player.png");
	result->obj[ go_monster] = loadTextureSheet( ren, "res/monster.png");
	result->obj[ go_apple] = loadTextureSheet( ren, "res/apple.png");
	result->obj[ go_flower] = loadTextureSheet( ren, "res/flower.png");
	//MAKE SURE THE 

	//TODO duplicate sprite sheet and color it
	//SDL_SetTextureColorMod( result->objectTextures[ TEXTURE_OBJ_MONSTER], 255, 150, 255);

	return result;
}

#undef SPRITE_TILE_LEN
