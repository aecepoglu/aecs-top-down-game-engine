#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "map.h"
#include "log.h"

#define MAP_VERSION 0

/* -----------
 *	FUNCTIONS
 */


/*	Reads the map file at 'path'.
	Returns the map read
*/
struct Map* readMapFile( char *path) {
	log1( "readMapFile( %s)\n", path);
	struct Map *m = (struct Map*)malloc( sizeof(struct Map) );
	FILE *fp = fopen( path, "rb");
	unsigned int x;
	assert( fp);

	//make sure map versions match
	int mapVersion;
	fread( &mapVersion, sizeof(unsigned int), 1, fp);

	assert(mapVersion == MAP_VERSION);

	//read dimensions first
	fread( &m->width, 		sizeof(unsigned int), 	1, fp);
	fread( &m->height, 		sizeof(unsigned int), 	1, fp);
	fread( &m->objListCount, 	sizeof(unsigned int), 	1, fp);
	fread( &m->objListSize, 	sizeof(unsigned int), 	1, fp);
	//can try to assert objListCount<objListSize
	
	//initialize the 1st dimension of map->tiles and map->objs
	m->objs = (struct object***)calloc( m->width, sizeof(struct object **));
	m->tiles = (enum terrainType**)calloc( m->width, sizeof(enum terrainType*));

	//initialize the objList array
	m->objList = (struct object**)calloc( m->objListSize, sizeof(struct object*));

	log1("initializing 2d objs map\n");
	//initialize the 2nd dimension of map->objs
	/* map->objs is a 2d map that has pointers to the actual object structs */
	for( x=0; x< m->width; x++)
		m->objs[x] = (struct object**)calloc( m->height, sizeof(struct object*));
		//all points of m->objs[x][y] should be 0 by default

	
	log1("reading objects-list. count %d size %d\n", m->objListCount, m->objListSize);
	
	//read the objList
	for( x=0; x< m->objListCount; x++) {
		struct object *obj = readObject( fp);

		m->objList[x] = obj;

		//setup pointers from the map->objs to objList items
		m->objs[ obj->pos.i][ obj->pos.j] = obj;
	}
	log1( "width %d\nheight %d\n", m->width, m->height);
	
	//read the map tiles
	for( x=0; x<m->width; x++) {
		m->tiles[x] = (enum terrainType*)malloc(sizeof(enum terrainType) * m->height);
		fread( m->tiles[x], sizeof(enum terrainType), m->height, fp);
	}
	fclose(fp);
	log1("map \"%s\" has been read\n%dx%d\n", path, m->width, m->height);
	m->filePath = path;
	return m;
}

struct object* findPlayer( struct Map *m) {
	log0( "Looking for player.\n");
	unsigned int i;
	for( i=0; i<m->objListCount; i++) {
		if( m->objList[i]->type == go_player) {
			log0 ( "Found player\n");
			return m->objList[i];
		}
	}
	log0 ( "Could not find player\n");
	return 0;
}


/* 	Saves the map to disk
	Map format : map-width map-height objListCount objListSize
		objList {x, y, type}
		map-tiles, column by column
 */
void saveMap( struct Map *map) {
	log1( "saveMap( ... to %s)\n", map->filePath);
	FILE *fp = fopen( map->filePath, "wb");
	assert( fp);

	//write map version first
	unsigned int mapVersion = MAP_VERSION;
	fwrite( &mapVersion, sizeof(unsigned int), 1, fp);

	//write dimensions first
	fwrite( &map->width,  sizeof(unsigned int), 1, fp);
	fwrite( &map->height, sizeof(unsigned int), 1, fp);

	//write the objects list, size first
	fwrite( &map->objListCount,	sizeof(unsigned int), 1, fp);
	fwrite( &map->objListSize,	sizeof(unsigned int), 1, fp);
	unsigned int i;
	for(i=0;   i<map->objListCount; i++)
		writeObject( fp, map->objList[i]);

	//now print the map terrain, column by column
	for(i=0; i<map->width; i++)
		fwrite( map->tiles[i], sizeof(enum terrainType), map->height, fp);
	fclose(fp);

	log1("Map saved to \"%s\"\n", map->filePath);
}

/* Create a new map that matches the given parameters
 */
struct Map* createNewMap( unsigned int width, unsigned int height) {
	log1( "createNewMap( %d, %d)\n", width, height);
	struct Map *m = (struct Map*)malloc(sizeof(struct Map));
	m->width = width;
	m->height = height;
	
	m->objListCount = 0;
	m->objListSize = 8;
	m->objList = (struct object**)calloc(m->objListSize, sizeof(struct object*));

	m->tiles = (enum terrainType**)calloc( width, sizeof(enum terrainType*));
	m->objs = (struct object***)calloc( width, sizeof(struct object**));
	
	unsigned int x,y;
	for(x=0; x<width; x++) {
		m->tiles[x] = (enum terrainType*)calloc( height, sizeof(enum terrainType));
		m->objs[x] = (struct object**)calloc( height, sizeof(struct object*));
		for(y=0; y<height; y++)
			m->tiles[x][y] = terrain_none;
	}

	log1( "createMap() over\n");
	return m;
}
