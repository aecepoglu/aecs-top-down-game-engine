/*
	Map utility definitions and functions
*/

#ifndef MAP_H
#define MAP_H

#include "object.h"

/* -----------
 * DEFINITIONS
 */
enum terrainType { terrain_none, terrain_wall};

struct Map {
	char *filePath;
	unsigned int width;
	unsigned int height;
	enum terrainType** tiles;
	struct object*** objs;
	struct object** objList;
	unsigned int objListCount;
	unsigned int objListSize;
};
/*
 objs:    /-----------/
         /     /     /
        /     /     /
	   /-----------/
	  /     /     /
	 /     /   ------> object
	/-----/-----/
*/



/* -----------
 *	FUNCTIONS
 */

#define TILE_CLEAR(_mapPtr,_x,_y) (_mapPtr->tiles[_x][_y] == terrain_none && _mapPtr->objs[_x][_y] == NULL)

/*	Reads the map file at 'path'.
	Returns the map read
*/
struct Map* readMapFile( char *path);

/*	Finds the player object in the given map and returns it
*/
struct object* findPlayer( struct Map* m);

/* Adds given object to given location.
*/
void addObject( struct object*, struct Map *map, int, int);

/* 	Saves the map to disk
 */
void saveMap( struct Map* map);

/* Create a new map that matches the given parameters
 */
struct Map* createNewMap( unsigned int width, unsigned int height);

bool checkMapValidity( struct Map *map);

#endif //MAP_H
