#include "textConsole.h"

int consoleX, consoleY;
char consoleBuf[CONSOLE_NUM_ROWS][CONSOLE_NUM_COLS_PLUS_1];

#define CONSOLE_BG_R 0
#define CONSOLE_BG_G 0
#define CONSOLE_BG_B 0
#define CONSOLE_BG_A 255

void textConsole_write( SDL_Renderer *renderer, SDL_Texture **fontMap, SDL_Texture *consoleTexture) {
	SDL_SetRenderTarget( renderer, consoleTexture);
	SDL_SetRenderDrawColor( renderer, CONSOLE_BG_R, CONSOLE_BG_G, CONSOLE_BG_B, CONSOLE_BG_A);
	SDL_RenderClear( renderer);

	int i,j;
	for( i=consoleY, j=0; j<CONSOLE_NUM_COLS; i = (i+1)%CONSOLE_NUM_ROWS, j++) {
		drawText( renderer, fontMap, consoleBuf[i], /*xPos*/0, /*yPos*/j*CONSOLE_FONT_HEIGHT, CONSOLE_FONT_WIDTH, CONSOLE_FONT_HEIGHT);
	}

	SDL_SetRenderTarget( renderer, NULL);
}

void textConsole_add( const char *text) {
	int i = 0;
	char c = text[i];
	while(1) {
		consoleBuf[ consoleY][ consoleX] = c;
		consoleX ++;
		i++;
		if( c == '\n' || c == '\0' || consoleX == CONSOLE_NUM_COLS) {
			consoleBuf[ consoleY][ consoleX] = '\0';
			consoleX = 0;
			consoleY = (consoleY + 1)%CONSOLE_NUM_ROWS;
		}
		if( c == '\0')
			break;;
		c = text[i];
	}
}

SDL_Texture* textConsole_init( SDL_Renderer *renderer) {
	consoleX = 0;
	consoleY = 0;

	int i;
	for( i=0; i<CONSOLE_NUM_ROWS; i++) {
		consoleBuf[i][0] = '\0';
	}
	
	SDL_Texture *result= SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, CONSOLE_WIDTH, CONSOLE_HEIGHT) ;

	SDL_SetRenderTarget( renderer, result);
	SDL_SetRenderDrawColor( renderer, CONSOLE_BG_R, CONSOLE_BG_G, CONSOLE_BG_B, CONSOLE_BG_A);
	SDL_RenderClear( renderer);

	SDL_SetRenderTarget( renderer, NULL);

	return result;
}
