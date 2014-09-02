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
			drawTexture( ren, font[(unsigned char)c], xPos + x*unitWidth, yPos + y*unitHeight, unitWidth, unitHeight);
			x++;
		}
		i++;
		c = text[i];
	}
}

SDL_Texture* getTextTexture( SDL_Renderer *ren, SDL_Texture **font, const char *text, int charWidth, int charHeight, int bgColor[4], int R, int G, int B, int *width, int *height) {
	
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

	int textWidth = charWidth*numCols;
	int textHeight = charHeight*numRows;

	SDL_Texture *result = SDL_CreateTexture( ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, textWidth, textHeight) ;
	SDL_SetRenderTarget( ren, result);
	SDL_SetRenderDrawColor( ren, bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	SDL_RenderClear( ren);

	SDL_Texture *container= SDL_CreateTexture( ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, textWidth, textHeight) ;
	SDL_SetRenderTarget( ren, container);
	SDL_SetRenderDrawColor( ren, 0,0,0,0);
	SDL_RenderClear( ren);

	SDL_SetRenderTarget( ren, container);
	drawText( ren, font, text, 0, 0, charWidth, charHeight);

	SDL_SetTextureColorMod( container, R, G, B);

	SDL_SetRenderTarget( ren, result);
	SDL_SetTextureBlendMode( container, SDL_BLENDMODE_BLEND);
	drawTexture( ren, container, 0, 0, textWidth, textHeight);

	SDL_SetRenderTarget( ren, 0);


	if( width)
		*width = textWidth;
	if( height)
		*height = textHeight;

	SDL_DestroyTexture( container);
	SDL_SetTextureBlendMode(result, SDL_BLENDMODE_BLEND);
	return result;
}
