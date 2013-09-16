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
#define TILELEN 16
enum tile { ground, wall, box};
typedef int bool;

struct map {
	unsigned int width;
	unsigned int height;
	enum tile **tiles;
};

struct player {
	SDL_Texture *texture;
	unsigned int x,y;
};
	


/* -----------
 *  VARIABLES 
 */
SDL_Window *win;
bool running;
SDL_Renderer *ren;
struct map *myMap;
const char *mapPath;
SDL_Texture* textures[2];
struct player player;
enum tile brush;



/* ------------
 *    MACROS
 */
#define log(...) fprintf(stdout, __VA_ARGS__)

void quit(char *msg) {
	fprintf(stderr, "Error msg: \"%s\"\n", msg);
	SDL_DestroyRenderer( ren);
	SDL_DestroyWindow( win);
	SDL_Quit();
	exit(0);
}

void init() {
	assert( SDL_Init(SDL_INIT_EVERYTHING) >= 0);
	win = SDL_CreateWindow("SDL Window", 0, 0, myMap->width *TILELEN, myMap->height *TILELEN, 0);
	assert( win);
	ren = SDL_CreateRenderer( win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert( ren);
}

void drawMap( struct map *m) {
	unsigned int x,y;
	for( x=0; x< m->width; x++) {
		for( y=0; y< m->height; y++) {
			SDL_Texture *t = textures[ m->tiles[x][y]/*returns 'enum tile*/ ];
			drawTexture( ren, t, x*TILELEN, y*TILELEN, TILELEN, TILELEN);
		}
	}
	drawTexture( ren, player.texture, player.x*TILELEN, player.y*TILELEN, TILELEN, TILELEN);
	//log("map drawn\n");
}

void draw() {
	log("draw\n");
	SDL_RenderClear( ren);
	drawMap(myMap);
	SDL_RenderPresent( ren);
}

void saveMap( const char *path) {
	FILE *fp = fopen( path, "wb");
	assert( fp);
	fwrite( &myMap->width, sizeof(unsigned int), 1, fp);
	fwrite( &myMap->height, sizeof(unsigned int), 1, fp);
	unsigned int x;
	for(x=0; x<myMap->width; x++)
		fwrite( myMap->tiles[x], sizeof(enum tile), myMap->height, fp);
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
