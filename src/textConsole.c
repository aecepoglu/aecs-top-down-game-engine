#include "textConsole.h"

int consoleX, consoleY;
char consoleBuf[CONSOLE_NUM_ROWS][CONSOLE_NUM_COLS_PLUS_1];

#define CONSOLE_BORDER_R 0
#define CONSOLE_BORDER_G 255
#define CONSOLE_BORDER_B 0
#define CONSOLE_BG_R 0
#define CONSOLE_BG_G 0
#define CONSOLE_BG_B 0
#define CONSOLE_BG_A 255

SDL_Texture *consoleBgTexture;

void textConsole_write( SDL_Renderer *renderer, SDL_Texture **fontMap, SDL_Texture *consoleTexture) {
	SDL_SetRenderTarget( renderer, consoleTexture);
	SDL_RenderClear( renderer);
	SDL_Rect rect = {.x=0, .y=0, .w=CONSOLE_WIDTH, .h=CONSOLE_HEIGHT};
	SDL_RenderCopy( renderer, consoleBgTexture, 0, &rect);

	int i,j;
	for( i=consoleY, j=0; j<CONSOLE_NUM_COLS; i = (i+1)%CONSOLE_NUM_ROWS, j++) {
		drawText( renderer, fontMap, consoleBuf[i], /*xPos*/CONSOLE_BORDER_THICKNESS, /*yPos*/CONSOLE_BORDER_THICKNESS + j*CONSOLE_FONT_HEIGHT, CONSOLE_FONT_WIDTH, CONSOLE_FONT_HEIGHT);
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

void textConsole_init( SDL_Renderer *renderer) {
	consoleX = 0;
	consoleY = 0;

	int i;
	for( i=0; i<CONSOLE_NUM_ROWS; i++) {
		consoleBuf[i][0] = '\0';
	}
	
	consoleBgTexture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, CONSOLE_WIDTH, CONSOLE_HEIGHT) ;

	SDL_SetRenderTarget( renderer, consoleBgTexture);
	SDL_SetRenderDrawColor( renderer, CONSOLE_BORDER_R, CONSOLE_BORDER_G, CONSOLE_BORDER_B, CONSOLE_BG_A);
	SDL_RenderClear( renderer);

	SDL_SetRenderDrawColor( renderer, CONSOLE_BG_R, CONSOLE_BG_G, CONSOLE_BG_B, CONSOLE_BG_A);
	SDL_Rect rect = {.x=CONSOLE_BORDER_THICKNESS, .y=CONSOLE_BORDER_THICKNESS, .w=CONSOLE_WIDTH-2*CONSOLE_BORDER_THICKNESS, .h=CONSOLE_HEIGHT-2*CONSOLE_BORDER_THICKNESS};
	SDL_RenderFillRect( renderer, &rect);


	textConsole_texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, CONSOLE_WIDTH, CONSOLE_HEIGHT) ;

	SDL_SetRenderTarget( renderer, textConsole_texture);
	SDL_SetRenderDrawColor( renderer, CONSOLE_BG_R, CONSOLE_BG_G, CONSOLE_BG_B, CONSOLE_BG_A);
	SDL_RenderClear( renderer);

	textConsole_write( renderer, NULL, textConsole_texture);

	SDL_SetRenderTarget( renderer, NULL);
}

void textConsole_destroy() {
	SDL_DestroyTexture(consoleBgTexture);
	SDL_DestroyTexture(textConsole_texture);
}
