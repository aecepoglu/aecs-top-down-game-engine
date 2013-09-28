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
enum terrainType { terrain_none, terrain_wall};
enum gameObjType { go_none, go_player, go_monster};
enum brushType { brush_none, brush_terrain, brush_object};
union brushType2 { enum terrainType terrain; enum gameObjType object; };
typedef int bool;


struct gameObject {
	SDL_Texture *texture;
	unsigned int x,y;
	enum gameObjType type;
};

struct map {
	char *filePath;
	unsigned int width;
	unsigned int height;
	enum terrainType** tiles;

	struct gameObject*** objs;
	struct gameObject** monsters;
	unsigned int monstersCount;
	unsigned int monstersSize;
};

	


/* -----------
 *  VARIABLES 
 */
bool running;
SDL_Window *win;
SDL_Renderer *ren;

/* textures map:
 0-empty
 1-terrain_none	/ padding for terrain = 1
 2-terrain_wall
 3-go_none		/ padding for gameObjType = 3
 4-go_player  
 5-go_monster
 */
SDL_Texture* textures[6]; 

struct map *myMap;

enum brushType mainBrush;
union brushType2 secondaryBrush;

int windowW=40, windowH=30, windowX=0, windowY=0;


/* ------------
 *    MACROS
 */
#define TILELEN 16
#define log(...) fprintf(stdout, __VA_ARGS__)
#define TEXTURE_MAP_TRN( t) textures[ 1 + (t)]
#define TEXTURE_MAP_OBJ( t) textures[ 3 + (t)]
#define MIN( x, y) ((x) < (y) ? (x) : (y))
#define MAX( x, y) ((x) > (y) ? (x) : (y))

void quit( const char *msg) {
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
	win = SDL_CreateWindow("SDL Window", 0, 0, windowW*TILELEN, windowH*TILELEN, 0);
	assert( win);
	ren = SDL_CreateRenderer( win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert( ren);
}

void drawMap() {
	unsigned int x,y,rx,ry;
	for( x=0, rx=windowX; rx< MIN(myMap->width, windowX+windowW); x++, rx++) {
		for( y=0, ry=windowY; ry< MIN(myMap->height, windowY+windowH); y++, ry++) {
			//draw the terrain if there is no object here; else, draw the object
			SDL_Texture *t = myMap->objs[rx][ry] == 0 ? TEXTURE_MAP_TRN( myMap->tiles[rx][ry]) : TEXTURE_MAP_OBJ( myMap->objs[rx][ry]->type);
			drawTexture( ren, t, x*TILELEN, y*TILELEN, TILELEN, TILELEN);
		}
	}
}

void draw() {
	SDL_RenderClear( ren);
	drawMap( myMap);
	SDL_RenderPresent( ren);
}

/*	Reads the map file at 'path'.
	Returns the map read
*/
struct map* readMapFile( char *path) {
	struct map *m = (struct map*)malloc( sizeof(struct map) );
	FILE *fp = fopen( path, "rb");
	unsigned int x;
	assert( fp);

	//read dimensions first
	fread( &m->width, 		sizeof(unsigned int), 	1, fp);
	fread( &m->height, 		sizeof(unsigned int), 	1, fp);
	fread( &m->monstersCount, 	sizeof(unsigned int), 	1, fp);
	fread( &m->monstersSize, 	sizeof(unsigned int), 	1, fp);
	
	//initialize the 1st dimension of map->tiles and map->objs
	m->objs = (struct gameObject***)calloc( m->width, sizeof(struct gameObject **));
	m->tiles = (enum terrainType**)calloc( m->width, sizeof(enum terrainType*));
	//initialize the monsters array
	m->monsters = (struct gameObject**)calloc( m->monstersCount, sizeof(struct gameObject*));
	//initialize the 2nd dimension of map->objs
	/* map->objs is a 2d map that has pointers to the actual gameObject structs */
	for( x=0; x< m->width; x++)
		m->objs[x] = (struct gameObject**)calloc( m->height, sizeof(struct gameObject*));
		//all points of m->objs[x][y] should be 0 by default

	//read the monsters
	struct gameObject* mon;
	for( x=0; x< m->monstersCount; x++) {
		mon = (struct gameObject*)malloc( sizeof(struct gameObject));
		fread( &mon->x		, sizeof(unsigned int), 	1, fp);
		fread( &mon->y		, sizeof(unsigned int), 	1, fp);
		fread( &mon->type	, sizeof(enum gameObjType),	1, fp);
		mon->texture = TEXTURE_MAP_OBJ( mon->type);

		m->monsters[x] = mon;
		//setup pointers from the map->objs to monsters items
		m->objs[ mon->x][ mon->y] = mon;
	}
	//log( "width %d\nheight %d\n", m->width, m->height);
	
	//read the map tiles
	for( x=0; x<m->width; x++) {
		m->tiles[x] = (enum terrainType*)malloc(sizeof(enum terrainType) * m->height);
		fread( m->tiles[x], sizeof(enum terrainType), m->height, fp);
	}
	fclose(fp);
	log("map \"%s\"  has been read\n%dx%d\n", path, m->width, m->height);
	m->filePath = path;
	return m;
}

/* 	Saves the map to disk
	Map format : map-width map-height monstersCount monstersSize
		monsters {x, y, type}
		map-tiles, column by column
 */
void saveMap() {
	FILE *fp = fopen( myMap->filePath, "wb");
	assert( fp);
	//write dimensions first
	fwrite( &myMap->width,  sizeof(unsigned int), 1, fp);
	fwrite( &myMap->height, sizeof(unsigned int), 1, fp);

	//write the objects list, size first
	fwrite( &myMap->monstersCount,	sizeof(unsigned int), 1, fp);
	fwrite( &myMap->monstersSize,	sizeof(unsigned int), 1, fp);
	unsigned int i;
	for(i=0;   i<myMap->monstersCount; i++) {
		fwrite( &myMap->monsters[i]->x,   	sizeof(unsigned int),     1, fp);
		fwrite( &myMap->monsters[i]->y,    	sizeof(unsigned int),     1, fp);
		fwrite( &myMap->monsters[i]->type, 	sizeof(enum gameObjType), 1, fp);
	}
	//now print the map terrain, column by column
	for(i=0; i<myMap->width; i++)
		fwrite( myMap->tiles[i], sizeof(enum terrainType), myMap->height, fp);
	fclose(fp);

	log("Map saved to \"%s\"\n", myMap->filePath);
}

/* Create a new map that matches the given parameters
 */
struct map* createNewMap( unsigned int width, unsigned int height) {
	struct map *m = (struct map*)malloc(sizeof(struct map));
	m->width = width;
	m->height = height;
	
	m->monstersCount = 0;
	m->monstersSize = 8;
	m->monsters = (struct gameObject**)calloc(m->monstersSize, sizeof(struct gameObject*));

	m->tiles = (enum terrainType**)calloc( width, sizeof(enum terrainType*));
	m->objs = (struct gameObject***)calloc( width, sizeof(struct gameObject**));
	
	unsigned int x,y;
	for(x=0; x<width; x++) {
		m->tiles[x] = (enum terrainType*)calloc( height, sizeof(enum terrainType));
		m->objs[x] = (struct gameObject**)calloc( height, sizeof(struct gameObject*));
		for(y=0; y<height; y++)
			m->tiles[x][y] = terrain_none;
	}
	return m;
}

void scrollScreen( int dX, int dY) {
	log("scroll, gived dx %d, dy %d, x %d, y %d, w %d, h %d\n", dX, dY, windowX, windowY, windowW, windowH);
	
	if( windowX + dX >= 0 && windowX + windowW + dX <= myMap->width)
		windowX += dX;
	if( windowY + dY >= 0 && windowY + windowH + dY <= myMap->height)
		windowY += dY;
	
	

	log("scrolled to (%d,%d)\n", windowX, windowY);
	draw();
}

void handleKey( SDL_KeyboardEvent *e) {
	//if( e->repeat)
	//	return;
	switch (e->keysym.sym) {
		case SDLK_s:
			/* saves the map, and continues */
			saveMap( myMap->filePath);
			break;
		case SDLK_q:
			/* terminates program without saving anything */
			running =0;
			break;
		case SDLK_1:
			/* switches brush to terrain.ground painting*/
			mainBrush = brush_terrain;
			secondaryBrush.terrain = terrain_none;
			break;
		case SDLK_2:
			/* switches brush to terrain.wall painting */
			mainBrush = brush_terrain;
			secondaryBrush.terrain = terrain_wall;
			break;
		case SDLK_3:
			/* switches brush to terrain.monster creation */
			mainBrush = brush_object;
			secondaryBrush.object = go_monster;
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
			log("Unrecognized key\n");
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

	if( mainBrush == brush_terrain) {
		if( myMap->tiles[x][y] != secondaryBrush.terrain) {
			myMap->tiles[x][y] = secondaryBrush.terrain;
			return 1;
		}
	}
	else if ( mainBrush == brush_object) {
		//add objects only if there is no object there
		if( myMap->objs[x][y] == 0 && secondaryBrush.object != go_none) {
			log( "Creating an object\n");
			//increase the array size if necessary
			if( myMap->monstersCount == myMap->monstersSize) {
				myMap->monstersSize *= 2;
				myMap->monsters = (struct gameObject**)realloc( myMap->monsters, sizeof(struct gameObject*) * myMap->monstersSize);
			}

			//create and initialize a monster
			struct gameObject *m;
			m = (struct gameObject*)malloc(sizeof(struct gameObject));
			m->type = secondaryBrush.object;
			m->x = x;
			m->y = y;
			m->texture = TEXTURE_MAP_OBJ( m->type);

			log( "\tcreated\n");
			
			//the 2d objs array maps to actual gameObject objects
			myMap->objs[x][y] = m;
			log( "\tpointer set\n");

			myMap->monsters[ myMap->monstersCount] = m;
			log( "\tinserted to array\n");
			myMap->monstersCount ++;
			return 1;
		}
	}
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
					    log("Window %d shown\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_HIDDEN:
					    log("Window %d hidden\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_EXPOSED:
					    //log("Window %d exposed\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_RESIZED:
					    log("Window %d resized to %dx%d\n",
					            e.window.windowID, e.window.data1,
					            e.window.data2);
						windowW = (e.window.data1-1) / TILELEN;
						windowH = (e.window.data2-1) / TILELEN;
					    break;
					case SDL_WINDOWEVENT_MINIMIZED:
					    log("Window %d minimized\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_MAXIMIZED:
					    log("Window %d maximized\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_RESTORED:
					    log("Window %d restored\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_CLOSE:
					    log("Window %d closed\n", e.window.windowID);
					    break;
				};
				break;
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
	
	//read textures
	TEXTURE_MAP_TRN( terrain_none) = loadTexture( ren, "res/ground.bmp");
	TEXTURE_MAP_TRN( terrain_wall) = loadTexture( ren, "res/wall.bmp");
	TEXTURE_MAP_OBJ( go_none) = loadTexture( ren, "res/empty.bmp");
	TEXTURE_MAP_OBJ( go_monster) = loadTexture( ren, "res/mon.bmp");
	TEXTURE_MAP_OBJ( go_player) = loadTexture( ren, "res/player.bmp");

	running = 1;
	run();

	log("Program is over.\nPeace\n");
	return 0;
}
