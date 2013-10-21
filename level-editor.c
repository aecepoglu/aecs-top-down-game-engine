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

#include "basic.h"
#include "texture.h"
#include "map.h"
#include "log.h"

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


struct map *myMap;

/* The drawing brushes. These are used to draw terrain and objects on map */
void (*brush)(unsigned int, unsigned int); //draws something on the given location
union { enum terrainType terrain; enum objType obj; enum aiType ai} secondaryBrush;

/* windowX,windowY : coordinates of current view in the whole map */
int windowW=40, windowH=30, windowX=0, windowY=0;


/* ------------
 *    MACROS
 */
#define TILELEN 16

void drawTerrain( unsigned int x, unsigned int y){
	if( myMap->tiles[x][y] != secondaryBrush.terrain) {
		myMap->tiles[x][y] = secondaryBrush.terrain;
		return 1;
	}
}

void drawObject( unsigned int x, unsigned int y){
	//add objects only if there is no object there
	if( myMap->objs[x][y] != 0 ) 
		return;
	
	log1( "Creating an object\n");
	//increase the array size if necessary
	if( myMap->objListCount == myMap->objListSize) {
		myMap->objListSize *= 2;
		myMap->objList = (struct object**)realloc( myMap->objList, sizeof(struct object*) * myMap->objListSize);
	}

	//create and initialize a monster
	struct object *obj = createObject( secondaryBrush.obj, x, y);

	log1( "\tcreated\n");
	
	//the 2d objs array maps to actual object objects
	myMap->objs[x][y] = obj;
	log1( "\tpointer set\n");

	myMap->objList[ myMap->objListCount] = obj;
	log1( "\tinserted to array\n");
	myMap->objListCount ++;
}

void drawAI( unsigned int x, unsigned int y) {
	struct object *obj = myMap->objs[x][y];
	
	//check if there is an intelligent object at the given location
	if( obj == 0)
		return;

	//remove the existing AI if necessary
	if( obj->ai != 0 && (obj->ai->type != secondaryBrush.ai || secondaryBrush.ai == ai_none) ){
		free(obj->ai);
		obj->ai = 0;
	}

	//now obj->ai = 0;
	if( secondaryBrush.ai != ai_none)
		


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
	ren = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert( renderer);
}

/* Draws the map and objects viewed on screen */
void drawMap() {
	unsigned int x,y,rx,ry;
	for( x=0, rx=windowX; rx< MIN(myMap->width, windowX+windowW); x++, rx++) {
		for( y=0, ry=windowY; ry< MIN(myMap->height, windowY+windowH); y++, ry++) {
			//draw the terrain if there is no object here; else, draw the object
			drawTexture( renderer,
				textures [
					myMap->objs[rx][ry] == 0
						? TEXTURE_MAP_TRN_TYPE( myMap->tiles[rx][ry])
						: TEXTURE_MAP_OBJ_TYPE( myMap->objs[rx][ry]->type)
					],
				x*TILELEN, y*TILELEN, TILELEN, TILELEN
				);
		}
	}
}

void draw() {
	SDL_RenderClear( renderer);
	drawMap( myMap);
	SDL_RenderPresent( renderer);
}

/* 	Saves the map to disk
	Map format : map-width map-height objListCount objListSize
		objList {x, y, type}
		map-tiles, column by column
 */

void scrollScreen( int dX, int dY) {
	log1("scroll, gived dx %d, dy %d, x %d, y %d, w %d, h %d\n", dX, dY, windowX, windowY, windowW, windowH);
	
	if( windowX + dX >= 0 && windowX + windowW + dX <= myMap->width)
		windowX += dX;
	if( windowY + dY >= 0 && windowY + windowH + dY <= myMap->height)
		windowY += dY;
	
	

	log1("scrolled to (%d,%d)\n", windowX, windowY);
	draw();
}

void handleKey( SDL_KeyboardEvent *e) {
	//if( e->repeat)
	//	return;
	switch (e->keysym.sym) {
		case SDLK_s:
			/* saves the map, and continues */
			saveMap( myMap);
			break;
		case SDLK_q:
			/* terminates program without saving anything */
			running =0;
			break;
		case SDLK_1:
			if( mainBrush == brush_terrain)
				secondaryBrush.terrain = terrain_none;
			else
				;//TODO secondaryBrush.obj = go_player;
			break;
		case SDLK_2:
			if( mainBrush == brush_terrain )
				secondaryBrush.terrain = terrain_wall;
			else
				secondaryBrush.obj = go_monster;
			break;
		case SDLK_t:
			mainBrush = brush_terrain;
			break;
		case SDLK_g:
			mainBrush = brush_object;
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
			log1("Unrecognized key\n");
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


	brush( x, y);



	//return 'true' only if there is a change in the map
	return 0;
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
					    log1("Window %d resized to %dx%d\n",
					            e.window.windowID, e.window.data1,
					            e.window.data2);
						windowW = (e.window.data1-1) / TILELEN;
						windowH = (e.window.data2-1) / TILELEN;
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
			myMap = readMapFile( mapPath);
	}



	init();
	
	
	textures = loadTextures( renderer);

	log0("All set and ready\nStarting...\n");

	running = 1;
	run();

	log0("Program over\nPeace\n");
	return 0;
}
