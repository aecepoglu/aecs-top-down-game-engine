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
	viewPos.i = 0;
	viewPos.j = 0;
	viewSize.i = 40;
	viewSize.j = 30;
	vectorAdd( &viewPos, &viewSize, &viewEnd);

	assert( SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_TIMER) >= 0);
	window = SDL_CreateWindow("aec-game", 0, 0, viewSize.i*TILELEN, viewSize.j*TILELEN, 0);
	assert( window);
	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert( renderer);
	bgroundTexture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, viewSize.i*TILELEN, viewSize.j*TILELEN);

}

/* Draws the map and objects viewed on screen */
void drawObjects() {
	unsigned i;
	struct Vector screenPos;
	for( i=0; i<myMap->objListCount; i++) {
		vectorSub( &myMap->objList[i]->pos, &viewPos, &screenPos);
		if( screenPos.i>=0 && screenPos.j>=0 && screenPos.i<viewSize.i && screenPos.j<viewSize.j ) {
			log2("drawing object %d\n", i);
			if( myMap->objList[i]->isDeleted ) //TODO remove these two lines after implementing object deletion
				continue;
			log2("drawing object %d for real\n", i);
			drawTexture( renderer, textures[ TEXTURE_MAP_OBJ_TYPE( myMap->objList[i]->type) ], screenPos.i*TILELEN, screenPos.j*TILELEN, TILELEN, TILELEN );
			drawTexture( renderer, textures[ TEXTURE_MAP_DIR( myMap->objList[i]->dir) ], screenPos.i*TILELEN, screenPos.j*TILELEN, TILELEN, TILELEN );
		}
	}
}

void drawBackground() {
	log2("generate background\n");
	SDL_DestroyTexture( bgroundTexture);
	bgroundTexture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, viewSize.i*TILELEN, viewSize.j*TILELEN) ;
	SDL_SetRenderTarget( renderer, bgroundTexture);

	drawTexture( renderer, textures[TEXTURE_TRN_NONE], 0, 0, windowW, windowH);

	unsigned int x,y,rx,ry;
	for( x=0, rx=viewPos.i; rx< MIN(myMap->width, viewEnd.i); x++, rx++)
    	for( y=0, ry=viewPos.j; ry< MIN(myMap->height, viewEnd.j); y++, ry ++)
    		drawTexture( renderer, textures [ TEXTURE_MAP_TRN_TYPE( myMap->tiles[rx][ry]) ],
    			x*TILELEN, y*TILELEN, TILELEN, TILELEN
    			);

	SDL_SetRenderTarget( renderer, 0); //reset
}

void draw() {
	log2("draw\n");
	SDL_RenderClear( renderer);
	//render background
	drawTexture( renderer, bgroundTexture, 0, 0, viewSize.i*TILELEN, viewSize.j*TILELEN);
	//render objects
	drawObjects( );
	SDL_RenderPresent( renderer);
}

bool scrollScreen( enum direction dir) {
	bool canScroll = false;
	switch(dir) {
		case dir_up:
			if(viewPos.j > 0)
				canScroll = true;
			break;
		case dir_left:
			if(viewPos.i > 0)
				canScroll = true;
			break;
		case dir_right:
			if(viewEnd.i < myMap->width )
				canScroll = true;
			break;
		case dir_down:
			if(viewEnd.j < myMap->height )
				canScroll = true;
			break;
	};

	if(canScroll) {
		struct Vector *dirVector = &dirVectors[dir];
		vectorAdd( dirVector, &viewPos, &viewPos);
		vectorAdd( dirVector, &viewEnd, &viewEnd);

		log1("scrolled to [(%d,%d), (%d,%d)]\n", viewPos.i, viewPos.j, viewEnd.i, viewEnd.j);
		drawBackground();
	}

	return canScroll;
}

void resizeView( int winW, int winH) {
	windowW = winW;
	windowH = winH;
	viewSize.i = (winW-1) / TILELEN;
	viewSize.j = (winH-1) / TILELEN;
	vectorAdd( &viewPos, &viewSize, &viewEnd);
}
