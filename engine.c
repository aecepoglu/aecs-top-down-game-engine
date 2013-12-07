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
void init() {
	windowW=40, windowH=30, windowX=0, windowY=0;

	assert( SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_TIMER) >= 0);
	window = SDL_CreateWindow("SDL Window", 0, 0, windowW*TILELEN, windowH*TILELEN, 0);
	assert( window);
	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert( renderer);
	bgroundTexture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, windowW*TILELEN, windowH*TILELEN);

}

/* Draws the map and objects viewed on screen */
void drawObjects() {
	unsigned i,x,y;
	for( i=0; i<myMap->objListCount; i++) {
		x = myMap->objList[i]->pos.i - windowX;
		y = myMap->objList[i]->pos.j - windowY;
		if( x>=0 && y>=0 && x<windowW && y<windowH ) {
			drawTexture( renderer, textures[ TEXTURE_MAP_OBJ_TYPE( myMap->objList[i]->type) ], x*TILELEN, y*TILELEN, TILELEN, TILELEN );
			drawTexture( renderer, textures[ TEXTURE_MAP_DIR( myMap->objList[i]->dir) ], x*TILELEN, y*TILELEN, TILELEN, TILELEN );
		}
	}
}

void drawBackground() {
	log0("generate background\n");
	SDL_DestroyTexture( bgroundTexture);
	bgroundTexture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, windowW*TILELEN, windowH*TILELEN) ;
	SDL_SetRenderTarget( renderer, bgroundTexture);

	unsigned int x,y,rx,ry;
	for( x=0, rx=windowX; rx< MIN(myMap->width, windowX+windowW); x++, rx++)
    	for( y=0, ry=windowY; ry< MIN(myMap->height, windowY+windowH); y++, ry ++)
    		drawTexture( renderer, textures [ TEXTURE_MAP_TRN_TYPE( myMap->tiles[rx][ry]) ],
    			x*TILELEN, y*TILELEN, TILELEN, TILELEN
    			);

	SDL_SetRenderTarget( renderer, 0); //reset
}

void draw() {
	log0("draw\n");
	SDL_RenderClear( renderer);
	//render background
	drawTexture( renderer, bgroundTexture, 0, 0, windowW*TILELEN, windowH*TILELEN);
	//render objects
	drawObjects( );
	SDL_RenderPresent( renderer);
}

/* 	Saves the map to disk
	Map format : map-width map-height objListCount objListSize
		objList {x, y, type}
		map-tiles, column by column
 */

void scrollScreen( int dX, int dY) {
	if( windowX + dX >= 0 && windowX + windowW + dX <= myMap->width)
		windowX += dX;
	if( windowY + dY >= 0 && windowY + windowH + dY <= myMap->height)
		windowY += dY;



	log1("scrolled to (%d,%d)\n", windowX, windowY);
	drawBackground();
}

