#include <SDL_image.h>
#include <assert.h>
#include <stdlib.h>

#include "texture.h"
#include "object.h"
#include "../util/log.h"

#define SPRITE_TILE_LEN 16

SDL_Texture *loadTexture( SDL_Renderer *ren, const char *path){
    log1( "\tLoading %s\n", path);
	SDL_Surface *img = IMG_Load( path);
	EXIT_IF(img == NULL, "Img %s not found", path);
	SDL_Texture *tex = SDL_CreateTextureFromSurface( ren, img);
	EXIT_IF( tex == NULL, "Texture is null");
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
		}
	}

	SDL_FreeSurface( blitSurf);
	return result;
}

/* takes a 1d list of textures, and returns a 2d array of textures
*/
SDL_Texture*** loadTexturesIntoTable( SDL_Renderer *ren, SDL_Texture **list, int listLen, int outNumRows, int outNumCols) {
	assert( (outNumRows * outNumCols) % listLen == 0);
	int factor = listLen / outNumRows;

	Uint32 textureFormat;
	int textureWidth, textureHeight;

	SDL_QueryTexture( list[0], &textureFormat, NULL, &textureWidth, &textureHeight );



	SDL_Texture ***table = (SDL_Texture***)calloc( outNumRows == 1 ? 2 : outNumRows, sizeof( SDL_Texture**));

	int row, col;
	for( row=0; row<outNumRows; row++) {
		table[ row] = (SDL_Texture**)calloc( outNumCols, sizeof( SDL_Texture*));
		for( col=0; col<outNumCols; col++) {
			SDL_Texture *rotatedTexture = SDL_CreateTexture(ren, textureFormat, SDL_TEXTUREACCESS_TARGET, textureWidth, textureHeight);
			SDL_SetTextureBlendMode(rotatedTexture, SDL_BLENDMODE_BLEND);
			SDL_SetRenderTarget(ren, rotatedTexture);

			SDL_RenderCopyEx( ren, list[row * factor + col % factor], NULL, NULL, ((col - col%factor)* 90), NULL, SDL_FLIP_NONE );
			
			table[ row][ col] = rotatedTexture;
		}
	}

	SDL_SetRenderTarget(ren, NULL);
	
	if (outNumRows == 1) {
		table[1] = table[0];
	}

	return table;
}

struct TextureSheet* loadObjTextures( SDL_Renderer *ren, const struct SpriteSpec *spec) {
	log1("\tLoading sheet %s\n", spec->path);
	SDL_Surface *img = IMG_Load(spec->path);
	assert( img != NULL);

	int numRotations = img->w / spec->width;
	int numStates = img->h / spec->height;
	log1( "\t\tWidth: %d, Height: %d. Num-states: %d, Num-rotations:%d\n", img->w, img->h, numStates, numRotations);

	struct TextureSheet *result = (struct TextureSheet*)malloc( sizeof( struct TextureSheet));

	SDL_Texture **texturesList = loadTextureSheet( ren, img, numStates, numRotations, spec->width, spec->height);

	result->numStates = numStates;
	result->spriteWidth = spec->width;
	result->spriteHeight = spec->height;
	result->tallness = spec->tallness;
	result->textures = loadTexturesIntoTable( ren, texturesList, numStates*numRotations, numStates, 4);

	free( texturesList);
	//FIXME free every item in texturesList
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

SDL_Rect _drawRect;
void drawTexture( SDL_Renderer *ren, SDL_Texture *tex, int x, int y, int w, int h) {
	_drawRect = (SDL_Rect){x, y, w, h };
	SDL_RenderCopy( ren, tex, 0, &_drawRect);
}



struct GameTextures* loadOrdinaryTextures( SDL_Renderer *ren) {
	log1("Loading textures\n");

	struct GameTextures *result = malloc( sizeof(struct GameTextures));


	char *trnPaths[terrain_NUM_ITEMS] = {
		[ terrain_dark 	] = "res/empty.png",
		[ terrain_wall 	] = "res/brick.png",
		[ terrain_gnd 	] = "res/ground.png",
		[ terrain_marked] = "res/specialGround.png",
	};

	result->trn = calloc( terrain_NUM_ITEMS, sizeof(SDL_Texture*));

	int i;
	for( i=0; i<terrain_NUM_ITEMS; i++)
		result->trn[ i] = loadTexture( ren, trnPaths[ i]);
	
	result->obj = NULL;
	result->objsCount = 0;

	result->font = loadFontSheet( ren, "res/font-sheet.png");

	result->highlitObjIndicator = loadTexture( ren, "res/selection_green.png");
	result->unidentifiedObj = loadTexture( ren, "res/qmark.png");
	result->emptyHeart = loadTexture( ren, "res/heart_empty.png");
	result->fullHeart = loadTexture( ren, "res/heart.png");
	result->inventory = loadTexture( ren, "res/inventory.png");

	return result;
}

void loadObjectTextures( SDL_Renderer *ren, struct GameTextures *textures, const struct SpriteSpecsList *specsList) {
	log1("Loading obj textures\n");

	textures->obj = calloc(specsList->size, sizeof(struct TextureSheet*));

	int i;
	for( i=0; i<specsList->size; i++)
		if(specsList->array[i] != NULL)
			textures->obj[i] = loadObjTextures(ren, specsList->array[i]);
		else
			textures->obj[i] = NULL;
	
	textures->objsCount = specsList->size;
}

void texture_freeTextureSheet( struct TextureSheet *sheet) {
	int j, k, tmpK;
	for( j=0; j<sheet->numStates; j++) {
		for( k=0; k<4; k++) {
			if( sheet->textures[j][k] != NULL) {
				SDL_DestroyTexture( sheet->textures[j][k]);

				/* in those rotations, there can be multiple elements pointing to the same texture.
					mark those that point to the same texture with NULL
				*/
				for( tmpK=k+1; tmpK<4; tmpK++)
					if( sheet->textures[j][tmpK] == sheet->textures[j][k])
						sheet->textures[j][tmpK] = NULL;
			}
		}
		free( sheet->textures[j]);
	}
	free( sheet->textures);
	free( sheet);
}

void freeObjectTextures( struct GameTextures *textures) {
	int i;

	printf("freeing textures\n");
	for( i=0; i<textures->objsCount; i++)
		if( textures->obj[i] != NULL)
			texture_freeTextureSheet( textures->obj[i]);

	free( textures->obj);
	
	textures->obj = NULL;
	textures->objsCount = 0;
}

void freeTextures( struct GameTextures* textures) {
	int i;

	freeObjectTextures(textures);

	for( i=0; i<terrain_NUM_ITEMS; i++) {
		SDL_DestroyTexture( textures->trn[ i]);
	}
	free( textures->trn);

	for( i=0; i<96; i++) {
		SDL_DestroyTexture( textures->font[i]);
	}
	free( textures->font);

	SDL_DestroyTexture( textures->highlitObjIndicator);
	SDL_DestroyTexture( textures->unidentifiedObj);
	SDL_DestroyTexture( textures->emptyHeart);
	SDL_DestroyTexture( textures->fullHeart);

	free( textures);
}

#undef SPRITE_TILE_LEN
