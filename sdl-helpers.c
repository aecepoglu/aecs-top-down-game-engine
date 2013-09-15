#include "sdl-helpers.h"
#include <assert.h>

/* Loads texture at given 'path' and returns it
*/
SDL_Texture *loadTexture( SDL_Renderer *ren, const char *path){
	SDL_Surface *img = SDL_LoadBMP( path);
	assert( img);
	SDL_Texture *tex = SDL_CreateTextureFromSurface( ren, img);
	assert( tex);
	SDL_FreeSurface( img);

	return tex;
}

/* Draws given texture 'tex' inside the given rectangle
*/
void drawTexture( SDL_Renderer *ren, SDL_Texture *tex, int x, int y, int w, int h) {
	SDL_Rect r = (SDL_Rect){x, y, w, h };
	SDL_RenderCopy( ren, tex, 0, &r);
}


