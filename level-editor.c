#include <SDL.h>
#include <SDL_version.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#include "sdl-helpers.h"

/* -----------
 * DEFINITIONS
 */
enum tile { ground, wall, box};
enum terrain { terrain_none, terrain_wall};
enum gameObjType { go_none, go_player, go_monster};
typedef int bool;

struct map {
	char *path;
	unsigned int width;
	unsigned int height;
	enum terrain** tiles;
	struct gameObject** objs;
};

struct gameObject {
	SDL_Texture *texture;
	unsigned int x,y;
	enum gameObjType type;
};
	


/* -----------
 *  VARIABLES 
 */
bool running;
SDL_Window *win;
SDL_Renderer *ren;

/* textures map:
 0-empty
 1-terrain_none / padding for terrain = 1
 2-terrain_wall
 3-go_player  / padding for gameObjType = 2
 4-go_monster

 */
SDL_Texture* textures[2]; 
#define TEX_PAD_TRN 1
#define TEX_PAD_OBJ 2

struct map myMap;
struct gameObject player;
struct gameObject* monsters;



/* ------------
 *    MACROS
 */
#define TILELEN 16
#define log(...) fprintf(stdout, __VA_ARGS__)

void quit(char *msg) {
	//TODO handle the termination properly.
	fprintf(stderr, "Error msg: \"%s\"\n", msg);
	SDL_DestroyRenderer( ren);
	SDL_DestroyWindow( win);
	SDL_Quit();
	exit(0);
}

/* Initializes the window, and creates a renderer object
 */
void init() {
	assert( SDL_Init(SDL_INIT_EVERYTHING) >= 0);
	win = SDL_CreateWindow("SDL Window", 0, 0, myMap->width *TILELEN, myMap->height *TILELEN, 0);
	assert( win);
	ren = SDL_CreateRenderer( win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert( ren);
}

void drawMap() {
	unsigned int x,y;
	for( x=0; x< m->width; x++) {
		for( y=0; y< m->height; y++) {
			//draw the game object if there is one; else, draw the terrain
			SDL_Texture *t = textures[ myMap->objs[x][y] == 0 ? (myMap->tiles[x][y]+TEX_PAD_TRN) : (myMap->objs[x][y]+TEX_PAD_OBJ) ];
			drawTexture( ren, t, x*TILELEN, y*TILELEN, TILELEN, TILELEN);
		}
	}
	//log("map drawn\n");
}

void draw() {
	log("draw\n");
	SDL_RenderClear( ren);
	drawMap(myMap);
	SDL_RenderPresent( ren);
}

/* Saves the map to disk
 */
void saveMap() {
	FILE *fp = fopen( myMap->filePath, "wb");
	assert( fp);
	//write dimensions first
	fwrite( &myMap->width,  sizeof(unsigned int), 1, fp);
	fwrite( &myMap->height, sizeof(unsigned int), 1, fp);
	fwrite( &objListSize,   sizeof(unsigned int), 1, fp);
	unsigned int i;
	for(i=0; i<objListSize; i++) {
		fwrite( &objList[i].x,    sizeof(unsigned int),     1, fp);
		fwrite( &objList[i].y,    sizeof(unsigned int),     1, fp);
		fwrite( &objList[i].type, sizeof(enum gameObjType), 1, fp);
	}
	for(i=0; i<myMap->width; i++)
		fwrite( myMap->tiles[i], sizeof(enum tile), myMap->height, fp);
	fclose(fp);

	log("Map saved to \"%s\"\n", path);
}


void handleKey( SDL_KeyboardEvent *e) {
	if( e->repeat)
		return;
	switch (e->keysym.sym) {
		case SDLK_s:
			saveMap( mapPath);
			break;
		case SDLK_q:
			running =0;
			break;
		case SDLK_1:
			brush = /*enum tile*/ground;
			break;
		case SDLK_2:
			brush = /*enum tile*/wall;
			break;
		case SDLK_3:
			log("'box' elements not yet supported\n");
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

	//return 'true' only if there is a change in the map
	if( brush != myMap->tiles[x][y]) {
		myMap->tiles[x][y] =  brush;
		return 1;
	}
	return 0;
}


int run() {
	SDL_Event e;
	while( running) {
		//while( SDL_PollEvent( &e)) {
		SDL_WaitEvent( &e);
		switch (e.type) {
			case SDL_QUIT:
				quit("Quitting");
				running =0;
				break;
			case SDL_KEYDOWN:
				log("key down\n");
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
				log("unhandled event type: %d\n", e.type);
				continue;
		};
		//}
		draw();
	}
}

/*	Reads the map file at 'path'.
	Returns the map read
*/
struct map* readMapFile( const char *path) {
	struct map *m = (struct map*)malloc( sizeof(struct map) );
	FILE *fp = fopen( path, "rb");
	assert( fp);

	fread( &m->width, sizeof(unsigned int), 1, fp);
	fread( &m->height, sizeof(unsigned int), 1, fp);
	log( "width %d\nheight %d\n", m->width, m->height);
	
	unsigned int x;
	m->tiles = (enum tile**)malloc(sizeof(enum tile*) * m->width);
	for( x=0; x<m->width; x++) {
		m->tiles[x] = (enum tile*)malloc(sizeof(enum tile) * m->height);
		fread( m->tiles[x], sizeof(enum tile), m->height, fp);
	}
	fclose(fp);
	log("map \"%s\" has been read\n", path);
	return m;
}


struct map* createNewMap( unsigned int width, unsigned int height) {
	struct map *m = (struct map*)malloc(sizeof(struct map));
	m->width = width;
	m->height = height;
	log("width %u\nheight %u\n", width, height);
	m->tiles = (enum tile**)malloc(sizeof(enum tile*)*width);
	int x,y;
	for(x=0; x<width; x++) {
		m->tiles[x] = (enum tile*)malloc(sizeof(enum tile)*height);
		for(y=0; y<height; y++)
			m->tiles[x][y] = ground;
	}
	return m;
}

int main( int argc, char *args[]) {
	//Default values
	myMap = 0;
	mapPath = 0;

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
	
	//read textures
	textures[ground] = loadTexture( ren, "res/bgnd.bmp");
	textures[wall] = loadTexture( ren, "res/wall.bmp");
	player.texture = loadTexture( ren, "res/img1.bmp");


	running = 1;
	run();

	log("Program is over.\nPeace\n");
	return 0;
}
