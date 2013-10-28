/*
	level-editor main code
*/

#include <SDL.h>
#include <SDL_version.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#include "object.h"
#include "texture.h"
#include "map.h"
#include "log.h"

#include "brush.h"

/* -----------
 * DEFINITIONS
 */



/* -----------
 *  VARIABLES 
 */
bool running;
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture **textures;
SDL_Texture *bgroundTexture;


struct Map *myMap;

/* The drawing brushes. These are used to draw terrain and objects on map */
brushFun *brush; //draws something on the given location
int brushVariant;
struct brushState* myBrushState;

/* windowX,windowY : coordinates of current view in the whole map */
int windowW=40, windowH=30, windowX=0, windowY=0;


/* ------------
 *    MACROS
 */
#define TILELEN 16


bool moveForward( struct Map *map, struct object* obj) {
	return 1;
}
bool turnLeft( struct Map *map, struct object *obj) {
	return 1;
}


bool defaultBrush( unsigned int x, unsigned int y, int type) {
	log0("brush unset\n");
	return 0;
}

bool drawTerrain( unsigned int x, unsigned int y, int type){
	if( myMap->tiles[x][y] != type) {
		myMap->tiles[x][y] = type;
		return 1;
	}
	else
		return 0;
}

bool drawObject( unsigned int x, unsigned int y, int type){
	//add objects only if there is no object there
	if( myMap->objs[x][y] != 0 ) 
		return 0;
	
	log1( "Creating an object\n");
	log0("obj %d/%d\n", myMap->objListCount, myMap->objListSize);
	//increase the array size if necessary
	if( myMap->objListCount == myMap->objListSize) {
		myMap->objListSize *= 2;
		log0("new size : %d\n", myMap->objListSize);
		myMap->objList = (struct object**)realloc( myMap->objList, sizeof(struct object*) * myMap->objListSize);
	}

	//create and initialize a monster
	struct object *obj = createObject( type, x, y);
	log0( "\tcreated %d, %d, %d\n", type, x, y);
	
	//the 2d objs array maps to actual object objects
	myMap->objs[x][y] = obj;
	log1( "\tpointer set\n");

	myMap->objList[ myMap->objListCount] = obj;
	myMap->objListCount ++;
	log1( "\tinserted to array\n");

	return 1;
}

bool drawAI( unsigned int x, unsigned int y, int type) {
	//TODO
}

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
	assert( SDL_Init(SDL_INIT_EVERYTHING) >= 0);
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
		x = myMap->objList[i]->x - windowX;
		y = myMap->objList[i]->y - windowY;
		if( x>=0 && y>=0 && x<windowW && y<windowH ) {
			drawTexture( renderer, textures[ TEXTURE_MAP_OBJ_TYPE( myMap->objList[i]->type) ], x*TILELEN, y*TILELEN, TILELEN, TILELEN );
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

void handleKey( SDL_KeyboardEvent *e) {
	switch (e->keysym.sym) {
		case SDLK_s:
			/* saves the map, and continues */
			saveMap( myMap);
			break;
		case SDLK_q:
			/* terminates program without saving anything */
			running =0;
			break;

		case SDLK_UP:
			scrollScreen( 0, -1);
			break;
		case SDLK_DOWN:
			scrollScreen( 0, +1);
			break;
		case SDLK_RIGHT:
			scrollScreen( +1, 0);
			break;
		case SDLK_LEFT:
			scrollScreen( -1, 0);
			break;
		default:
			brush = updateBrushState( &myBrushState, &brushVariant, e->keysym.sym);
			break;
	};
}

/* The mouse button-event and motion-events are handled the same
 * I just need to extract the x,y from the event-data.
 */
bool handleMouse( SDL_MouseButtonEvent *e, SDL_MouseMotionEvent *e2) {
	unsigned int x,y;
	if( e) {
		x = e->x / TILELEN;
		y = e->y / TILELEN;
	}
	else {
		if( ! e2->state) //if no buttons are pressed
			return 0;
		x = e2->x / TILELEN;
		y = e2->y / TILELEN;
	}

	x += windowX;
	y += windowY;

	//if the mouse is outside the map, don't do anything
	if( x > myMap->width || y > myMap->height)
		return 0;


	return brush( x, y, brushVariant);
}


int run() {
	draw();
	SDL_Event e;
	while( running) {
		//while( SDL_PollEvent( &e)) {
		//}
		SDL_WaitEvent( &e);
		switch (e.type) {
			case SDL_WINDOWEVENT:
				switch( e.window.event) {
					case SDL_WINDOWEVENT_SHOWN:
					    log1("Window %d shown\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_HIDDEN:
					    log1("Window %d hidden\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_EXPOSED:
					    //log1("Window %d exposed\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_RESIZED:
					    log0("Window %d resized to %dx%d\n",
					            e.window.windowID, e.window.data1,
					            e.window.data2);
						windowW = (e.window.data1-1) / TILELEN;
						windowH = (e.window.data2-1) / TILELEN;
						drawBackground();
					    break;
					case SDL_WINDOWEVENT_MINIMIZED:
					    log1("Window %d minimized\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_MAXIMIZED:
					    log1("Window %d maximized\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_RESTORED:
					    log1("Window %d restored\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_CLOSE:
					    log1("Window %d closed\n", e.window.windowID);
					    break;
					default:
						//unhandled window event
						continue;
				};
				break;
			case SDL_QUIT:
				quit("Quitting");
				running =0;
				break;
			case SDL_KEYDOWN:
				log1("key down\n");
				handleKey( (SDL_KeyboardEvent*)&e);
				break;
			case SDL_MOUSEBUTTONDOWN:
				handleMouse( (SDL_MouseButtonEvent*)&e, 0);
				break;
			case SDL_MOUSEMOTION:
				if( handleMouse( 0, (SDL_MouseMotionEvent*)&e) )
					break;
			case SDL_KEYUP:
			case SDL_MOUSEBUTTONUP:
				/*don't do anything for those events*/
				continue;
			default:
				log1("unhandled event type: %d\n", e.type);
				continue;
		};
		draw();
	}
}



int main( int argc, char *args[]) {
	//Default values
	myMap = 0;
	char *mapPath = 0;

	//Handle cmd-line arguments
	int argi = 1;
	while(argi < argc) {
		if( args[argi][0] == '-') {
			char c = args[argi][1];
			switch (c) {
				case 'm':
					mapPath = args[argi+1];
					argi++;
					break;
				case 'n':
					myMap = createNewMap( (unsigned int)atoi(args[argi+1]), (unsigned int)atoi(args[argi+2]));
					myMap->filePath = mapPath;
					argi+=2;
					break;
				default:
					argi = argc; //break out from the loop
					break;
			};
			argi++;
		}
		else {
			fprintf(stderr, "Unrecognized argument at index %d: \"%s\"", argi, args[argi]);
			exit(1);
		}
	}

	if(myMap == 0) {
		if(mapPath == 0) {
			fprintf( stderr, "There is no map. Open a map with '-m map-path' or create new map with '-m map-path -n map-width map-height'\n");
			exit(0);
		}
		else
			log1( "reading map file %s\n", mapPath);
			log0( "reading map file %s\n", mapPath);
			myMap = readMapFile( mapPath);
	}



	init();
	
	
	textures = loadTextures( renderer);
	myBrushState = &initialBrushState;
	brush = defaultBrush;

	log0("All set and ready\nStarting...\n");

	running = 1;
	run();

	log0("Program over\nPeace\n");
	return 0;
}
