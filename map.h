/*
	Map utility definitions and functions
*/

#ifndef MAP_H
#define MAP_H

#include "basic.h"

/* -----------
 * DEFINITIONS
 */
enum terrainType { terrain_none, terrain_wall};

struct map {
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

	


/* ------------
 *    MACROS
 */
#define TILELEN 16



/* -----------
 *	FUNCTIONS
 */


/*	Reads the map file at 'path'.
	Returns the map read
*/
struct map* readMapFile( char *path);

/* 	Saves the map to disk
 */
void saveMap( struct map* map); 

/* Create a new map that matches the given parameters
 */
struct map* createNewMap( unsigned int width, unsigned int height); 

#endif //MAP_H
