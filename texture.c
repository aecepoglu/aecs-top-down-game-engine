#include "texture.h"
#include <SDL_image.h>
#include <assert.h>
#include <stdlib.h>

#include "log.h"

SDL_Texture *loadTexture( SDL_Renderer *ren, const char *path){
    log0( "Loading %s\n", path);
	SDL_Surface *img = IMG_Load( path);
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
	SDL_Texture **textures = calloc( TEXTURES_COUNT, sizeof(SDL_Texture*) );
	textures[ TEXTURE_BAD 			] = loadTexture( ren, "res/qmark.png");
	textures[ TEXTURE_TRN_NONE 		] = loadTexture( ren, "res/ground.bmp");
	textures[ TEXTURE_TRN_WALL 		] = loadTexture( ren, "res/brick.png");
	textures[ TEXTURE_OBJ_MONSTER 	] = loadTexture( ren, "res/monster.png");
	textures[ TEXTURE_OBJ_PLAYER 	] = loadTexture( ren, "res/player.png");
	textures[ TEXTURE_OBJ_APPLE     ] = loadTexture( ren, "res/apple.png");
	textures[ TEXTURE_EYE_RIGHT 	] = loadTexture( ren, "res/monster_eye_right.png");
	textures[ TEXTURE_EYE_UP        ] = loadTexture( ren, "res/monster_eye_up.png");
	textures[ TEXTURE_EYE_LEFT      ] = loadTexture( ren, "res/monster_eye_left.png");
	textures[ TEXTURE_EYE_DOWN   	] = loadTexture( ren, "res/monster_eye_down.png");


	SDL_SetTextureColorMod( textures[ TEXTURE_OBJ_MONSTER], 255, 150, 255);


	return textures;
}
