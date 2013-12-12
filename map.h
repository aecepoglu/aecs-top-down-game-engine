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


/*	Reads the map file at 'path'.
	Returns the map read
*/
struct Map* readMapFile( char *path);

/*	Finds the player object in the given map and returns it
*/
struct object* findPlayer( struct Map* m);

/* 	Saves the map to disk
 */
void saveMap( struct Map* map);

/* Create a new map that matches the given parameters
 */
struct Map* createNewMap( unsigned int width, unsigned int height);

#endif //MAP_H
