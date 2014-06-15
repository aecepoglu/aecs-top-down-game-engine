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
	windowW = viewSize.i * TILELEN;
	windowH = viewSize.j * TILELEN;

	vectorAdd( &viewEnd, &viewPos, &viewSize  );

	assert( SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_TIMER) >= 0);
	window = SDL_CreateWindow("sdl-window", 0, 0, viewSize.i*TILELEN, viewSize.j*TILELEN, 0);
	assert( window);
	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert( renderer);
	bgroundTexture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, viewSize.i*TILELEN, viewSize.j*TILELEN);

}

/* Draws the map and objects viewed on screen */
void drawObjects() {
	unsigned i;
	struct Vector screenPos;
	struct object *obj;

	struct object **newObjList = (struct object**)calloc( myMap->objListSize, sizeof( struct object*));
	int newCount = 0;

	for( i=0; i<myMap->objListCount; i++) {
		obj = myMap->objList[i];
		if( ! obj->isDeleted) {
			newObjList[ newCount] = obj;
			newCount ++;

			vectorSub( &screenPos, &myMap->objList[i]->pos, &viewPos );
			if( screenPos.i>=0 && screenPos.j>=0 && screenPos.i<viewSize.i && screenPos.j<viewSize.j ) {
				log3("drawing object %d\n", i);
				//TODO use visual state only: don't do health check. if object died after attacking, set visual state in the hit() function
				drawTexture( renderer, 
					textures->obj[obj->type][ obj->health > 0 ? obj->visualState : 0 ][obj->dir], 
					screenPos.i*TILELEN, screenPos.j*TILELEN, TILELEN, TILELEN );
			}
		}
	}

	free( myMap->objList);
	myMap->objList = newObjList;
	myMap->objListCount = newCount;
}

void drawBackground() {
	log3("generate background\n");
	SDL_DestroyTexture( bgroundTexture);
	bgroundTexture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, viewSize.i*TILELEN, viewSize.j*TILELEN) ;
	SDL_SetRenderTarget( renderer, bgroundTexture);

	drawTexture( renderer, textures->trn[TEXTURE_TRN_NONE], 0, 0, windowW, windowH);

	unsigned int x,y,rx,ry;
	for( x=0, rx=viewPos.i; rx< MIN(myMap->width, viewEnd.i); x++, rx++)
    	for( y=0, ry=viewPos.j; ry< MIN(myMap->height, viewEnd.j); y++, ry ++)
    		drawTexture( renderer, textures->trn[ myMap->tiles[rx][ry] ],
    			x*TILELEN, y*TILELEN, TILELEN, TILELEN
    			);

	SDL_SetRenderTarget( renderer, 0); //reset
}

void draw() {
	log3("draw\n");
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
		vectorAdd( &viewPos, &viewPos, dirVector );
		vectorAdd( &viewEnd, &viewEnd, dirVector );

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
	vectorAdd( &viewEnd, &viewPos, &viewSize );
}

