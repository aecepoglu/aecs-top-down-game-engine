/*
	Map utility definitions and functions
*/

#ifndef MAP_H
#define MAP_H

#include "object.h"
#include "vector.h"

/* -----------
 * DEFINITIONS
 */
enum terrainType { terrain_gnd, terrain_wall, terrain_dark };

struct BasePfNode {
	struct Vector pos;
	struct BasePfNode *neighbours[4];
};

struct Map {
	char *filePath;

	unsigned int width;
	unsigned int height;

	enum terrainType** tiles;

	struct BasePfNode ***pfBase;

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

#define GET_PF_POS(x) (x != NULL ? &(x->pos) : NULL)
#define TILE_CLEAR(_mapPtr,_x,_y) (_mapPtr->tiles[_x][_y] == terrain_gnd && _mapPtr->objs[_x][_y] == NULL)

/*	Reads the map file at 'path'.
	Returns the map read
*/
struct Map* readMapFile( const char *path);

/* Adds given object to given location.
*/
void addObject( struct object*, struct Map *map, int x, int y);

/* 	Saves the map to disk
 */
bool saveMap( struct Map* map);

/* Create a new map that matches the given parameters
 */
struct Map* createNewMap( unsigned int width, unsigned int height);

bool checkMapValidity( struct Map *map);

struct BasePfNode ***createPfBase( enum terrainType **tiles, unsigned int width, unsigned int height);

void freeMap( struct Map *map);

#endif //MAP_H
