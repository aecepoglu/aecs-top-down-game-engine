#include "text.h"
#include "basic.h"

void drawText( SDL_Renderer *ren, SDL_Texture **font, const char *text, int xPos, int yPos, int unitWidth, int unitHeight) {
	int i = 0;
	int x =0, y=0;
	char c = text[i];
	while( c != '\0') {
		if( c == '\n') {
			y++;
			x=0;
		}
		else {
			drawTexture( ren, font[c-32], xPos + x*unitWidth, yPos + y*unitHeight, unitWidth, unitHeight);
			x++;
		}
		i++;
		c = text[i];
	}
}

SDL_Texture* getTextTexture( SDL_Renderer *ren, SDL_Texture **font, const char *text, int charWidth, int charHeight, int R, int G, int B, int *width, int *height) {
	
	int numRows=1, numCols=0;
	int i=0, x=0;
	char c = text[i];
	while( c != '\0') {
		if( c == '\n') {
			numRows++;
			numCols = MAX( numCols, x);
			x=0;
		}
		else {
			x++;
		}
		i++;
		c = text[i];
	}
	numCols = MAX( numCols, x);

	SDL_Texture *result = SDL_CreateTexture( ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, charWidth*numCols, charHeight*numRows) ;
	SDL_SetRenderTarget( ren, result);

	drawText( ren, font, text, 0, 0, charWidth, charHeight);

	SDL_SetRenderTarget( ren, 0);

	SDL_SetTextureColorMod( result, R, G, B);

	if( width)
		*width = charWidth*numCols;
	if( height)
		*height = charHeight*numRows;

	return result;
}
