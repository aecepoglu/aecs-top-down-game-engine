#include "engine.h"

void quit( const char *msg) {
	//TODO handle the termination properly.
	fprintf(stderr, "Error msg: \"%s\"\n", msg);
	SDL_DestroyRenderer( renderer);
	SDL_DestroyWindow( window);
	SDL_Quit();
	exit(0);
}

/* Initializes the window, and creates a renderer object
 */
void init( int offsetX, int offsetY, int winWidth, int winHeight) {
	viewPos.i = 0;
	viewPos.j = 0;
	resizeView( offsetX, offsetY, winWidth, winHeight);

	assert( SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_TIMER | SDL_INIT_AUDIO) >= 0);
	window = SDL_CreateWindow("sdl-window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winWidth, winHeight, 0);
	assert( window);
	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert( renderer);

	bgroundTexture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, viewSize.i*TILELEN, viewSize.j*TILELEN);
}

/*
*/

void resizeView( int posX, int posY, int winW, int winH) {
	windowW = winW;
	windowH = winH;
	viewSize.i = (winW-1-posX) / TILELEN;
	viewSize.j = (winH-1-posY) / TILELEN;
	log1("size is %dx%d\n", viewSize.i, viewSize.j);
	vectorAdd( &viewEnd, &viewPos, &viewSize );
}

