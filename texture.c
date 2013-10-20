#include "texture.h"
#include <assert.h>

SDL_Texture *loadTexture( SDL_Renderer *ren, const char *path){
	SDL_Surface *img = SDL_LoadBMP( path);
	assert( img);
	SDL_Texture *tex = SDL_CreateTextureFromSurface( ren, img);
	assert( tex);
	SDL_FreeSurface( img);

	return tex;
}

void drawTexture( SDL_Renderer *ren, SDL_Texture *tex, int x, int y, int w, int h) {
	SDL_Rect r = (SDL_Rect){x, y, w, h };
	SDL_RenderCopy( ren, tex, 0, &r);
}

SDL_Texture** loadTextures( SDL_Renderer *ren) {
	SDL_Texture **textures = calloc( 5, sizeof(SDL_Texture*) );
	textures[ TEXTURE_BAD 			] = loadTexture( ren, "res/ground.bmp");
	textures[ TEXTURE_TRN_NONE 		] = loadTexture( ren, "res/ground.bmp");
	textures[ TEXTURE_TRN_WALL 		] = loadTexture( ren, "res/wall.bmp");
	textures[ TEXTURE_OBJ_PLAYER 	] = loadTexture( ren, "res/player.bmp");
	textures[ TEXTURE_OBJ_MONSTER 	] = loadTexture( ren, "res/mon.bmp");
	
	return textures;
}
