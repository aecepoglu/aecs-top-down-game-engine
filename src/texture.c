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

/* Loads a texture sheet into a 1d array*/
SDL_Texture** loadTextureSheet( SDL_Renderer *ren, SDL_Surface *sheet, int inNumRows, int inNumCols, int unitWidth, int unitHeight) {
	assert( sheet->w == inNumCols * unitWidth);
	assert( sheet->h == inNumRows * unitHeight);

	SDL_Texture **result = (SDL_Texture**)calloc( inNumRows * inNumCols, sizeof( SDL_Texture **));

	SDL_Surface *blitSurf = SDL_CreateRGBSurface(0, unitWidth, unitHeight, sheet->format->BitsPerPixel, sheet->format->Rmask, sheet->format->Gmask, sheet->format->Bmask, sheet->format->Amask);
	
	SDL_Rect surfRect;
	surfRect.w = unitWidth;
	surfRect.h = unitHeight;

	int row, col;
	for( row=0; row<inNumRows; row++) {
		surfRect.y = row * unitHeight;
		for( col=0; col<inNumCols; col++) {
			surfRect.x = col * unitWidth;

			SDL_FillRect( blitSurf, NULL, 0);
			if (SDL_BlitSurface( sheet, &surfRect, blitSurf, NULL) != 0)
				log0("SDL_BlitSurface failed: %s\n", SDL_GetError());
			result[ row*inNumCols + col] = SDL_CreateTextureFromSurface( ren, blitSurf);
			log3("loadTextureSheet: %d <- (%d, %d)\n", row*inNumCols + col, surfRect.x, surfRect.y);
		}
	}

	SDL_FreeSurface( blitSurf);
	return result;
}

/* takes a 1d list of textures, and returns a 2d array of textures
*/
SDL_Texture*** loadTexturesIntoTable( SDL_Texture **list, int listLen, int outNumRows, int outNumCols) {
	assert( (outNumRows * outNumCols) % listLen == 0);
	int rowJumpLen = listLen / outNumRows;

	SDL_Texture ***table = (SDL_Texture***)calloc( outNumRows, sizeof( SDL_Texture**));
	int row, col;
	for( row=0; row<outNumRows; row++) {
		table[ row] = (SDL_Texture**)calloc( outNumCols, sizeof( SDL_Texture*));
		for( col=0; col<outNumCols; col++) {
			table[ row][ col] = list[ row*rowJumpLen + col%rowJumpLen];
			log3("loadTexturesIntoTablo: table[%d,%d] <- #%d\n", row, col, row*rowJumpLen + col%rowJumpLen);
		}
	}

	return table;
}

struct TextureSheet* loadObjTextures( SDL_Renderer *ren, const char *path) {
    log1( "\tLoading sheet %s\n", path);
	SDL_Surface *img = IMG_Load( path);
	assert( img != NULL);

	int numRotations = img->w / SPRITE_TILE_LEN;
	int numStates = img->h / SPRITE_TILE_LEN;
	assert(numStates >= 2);
	log0( "\t\tWidth: %d, Height: %d. Num-states: %d, Num-rotations:%d\n", img->w, img->h, numStates, numRotations);

	struct TextureSheet *result = (struct TextureSheet*)malloc( sizeof( struct TextureSheet));
	
	SDL_Texture **texturesList = loadTextureSheet( ren, img, numStates, numRotations, SPRITE_TILE_LEN, SPRITE_TILE_LEN);

	result->numStates = numStates;
	result->textures = loadTexturesIntoTable( texturesList, numStates*numRotations, numStates, 4);

	free( texturesList);
	SDL_FreeSurface( img);

	return result;
}

SDL_Texture** loadFontSheet( SDL_Renderer *ren, const char *path) {
	SDL_Surface *img = IMG_Load( path);
	assert( img != NULL);

	SDL_Texture **result = loadTextureSheet( ren, img, 5, 32, 6, 8);

	SDL_FreeSurface( img);
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

	result->trn[ TEXTURE_TRN_NONE 		] = loadTexture( ren, "res/empty.png");
	result->trn[ TEXTURE_TRN_GND 		] = loadTexture( ren, "res/ground.png");
	result->trn[ TEXTURE_TRN_WALL 		] = loadTexture( ren, "res/brick.png");

	result->obj = calloc( go_NUM_ITEMS, sizeof(struct TextureSheet*));

	result->obj[ go_player] 		= loadObjTextures( ren, "res/player.png");
	result->obj[ go_leftTurner] 	= loadObjTextures( ren, "res/left-turner.png");
	result->obj[ go_flower] 		= loadObjTextures( ren, "res/flower.png");
	result->obj[ go_creeperPlant] 	= loadObjTextures( ren, "res/creeper.png");
	result->obj[ go_peekaboo] 		= loadObjTextures( ren, "res/peekaboo.png");
	result->obj[ go_weepingAngel] 	= loadObjTextures( ren, "res/weepingAngel.png");

	result->obj[ go_apple] 			= loadObjTextures( ren, "res/apple.png");
	result->obj[ go_door] 			= loadObjTextures( ren, "res/door.png");
	result->obj[ go_button] 		= loadObjTextures( ren, "res/but.png");
	result->obj[ go_switch] 		= loadObjTextures( ren, "res/sw.png");

	result->font = loadFontSheet( ren, "res/font-sheet.png");

	result->highlitObjIndicator = loadTexture( ren, "res/selection_green.png");
	
	return result;
}

void freeTextures( struct GameTextures* textures) {
	int i,j,k,tmpK;
	for( i=0; i<go_NUM_ITEMS; i++) {
		for( j=0; j<textures->obj[i]->numStates; j++) {
			for( k=0; k<4; k++) {
				if( textures->obj[i]->textures[j][k] != NULL) {
					SDL_DestroyTexture( textures->obj[i]->textures[j][k]);

					/* in those rotations, there can be multiple elements pointing to the same texture.
						mark those that point to the same texture with NULL
					*/
					for( tmpK=k+1; tmpK<4; tmpK++)
						if( textures->obj[i]->textures[j][tmpK] == textures->obj[i]->textures[j][k])
							textures->obj[i]->textures[j][tmpK] = NULL;
				}
			}
			free( textures->obj[i]->textures[j]);
		}
		free( textures->obj[i]->textures);
		free( textures->obj[i]);
	}
	free( textures->obj);

	for( i=0; i<TEXTURES_COUNT_TERRAIN; i++) {
		SDL_DestroyTexture( textures->trn[ i]);
	}
	free( textures->trn);

	for( i=0; i<96; i++) {
		SDL_DestroyTexture( textures->font[i]);
	}
	free( textures->font);

	SDL_DestroyTexture( textures->highlitObjIndicator);

	free( textures);
}

#undef SPRITE_TILE_LEN
