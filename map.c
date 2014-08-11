#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "array.h"

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

	//generate the base path-finding
	m->pfBase = createPfBase( m->tiles, m->width, m->height);

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



void addObject( struct object* obj, struct Map *map, int x, int y) {
	ARRAY_ADD( map->objList, obj, map->objListCount, map->objListSize, sizeof( struct object));
	map->objs[x][y] = obj;
}


void saveMap( struct Map *map) {
	log1( "saveMap to '%s'\n", map->filePath);

	//confirm the ground tiles are connected
	if( ! checkMapValidity(map) ) {
		log0("\tMap is not valid. Not gonna save\n");
		return;
	}


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

	log0("Map saved to \"%s\"\n", map->filePath);
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
			m->tiles[x][y] = terrain_gnd;
	}

	log1( "createMap() over\n");
	return m;
}

bool checkMapValidity( struct Map *map) {
	log0("Validating map\n");
	bool result;

	unsigned int x,y;

	int playerCount = 0;
	int groundCount = 0;

	int **connectedTiles = (int**)calloc( map->width, sizeof(int*));
	for(x=0; x<map->width; x++) {
		connectedTiles[x] = (int*)calloc( map->height, sizeof(int));
		for(y=0; y<map->height; y++) {
			if( map->tiles[x][y] == terrain_gnd) {

				if( x == map->width -1 || x == 0 || y == map->height - 1 || y == 0) {
					log0("\tGround tile at position %d,%d is at the edge. There cannot be ground tiles at map edges. Edges must be covered with walls.\n", x, y);
					goto freeConnectedTiles;
				}
					

				groundCount ++;
				connectedTiles[x][y] = 0;
			}
			else
				connectedTiles[x][y] = -1;

			if( map->objs[x][y] != 0 && map->objs[x][y]->type == go_player)
				playerCount ++;
		}
	}

	if( playerCount != 1) {
		log0("\tplayer count is %d. must be 1.\n", playerCount);
		result = false;
		goto freeConnectedTiles;
	}
	
	if( groundCount == 0) {
		log0("\tgroundCount is %d. must be > 0.\n", groundCount);
		result = false;
		goto freeConnectedTiles;
	}


	int groupLookupSize = 64;
	int *groupLookup = (int*)calloc( groupLookupSize, sizeof(int));
	int connectedGroupsCount = 0;
	int currentGroupIndex = 0;
	for(x=0; x<map->width; x++) {
		for(y=0; y<map->height; y++) {
			if(connectedTiles[x][y] >= 0) {
				if(connectedTiles[x][y] == 0) {
					connectedGroupsCount ++;
					currentGroupIndex ++;
					if(currentGroupIndex >= groupLookupSize) {
						groupLookupSize *= 2;
						groupLookup = realloc( groupLookup, sizeof(int) * groupLookupSize);
					}
					connectedTiles[x][y] = currentGroupIndex;
				}

				if(connectedTiles[x+1][y] == 0)
					connectedTiles[x+1][y] = connectedTiles[x][y];
				
				if(connectedTiles[x][y+1] == 0)
					connectedTiles[x][y+1] = connectedTiles[x][y];
				else if(connectedTiles[x][y+1] != -1){
					int k = connectedTiles[x][y+1];
					while(k != connectedTiles[x][y]) {
						if(groupLookup[k] == 0){
							connectedGroupsCount --;
							groupLookup[ connectedTiles[x][y+1] ] = connectedTiles[x][y];
							log2("\t\tsetting lookup %d to %d\n", connectedTiles[x][y+1], connectedTiles[x][y]);
							break;
						}
						else
							k = groupLookup[k];
					}
				}
			}
		}
	}

	if( connectedGroupsCount > 1) {
		log0("\tThere must be only 1 connected group of ground tiles. Map has %d\n", connectedGroupsCount);
		result = false;
		goto end;
	}
	result = true;

	end:
	free(groupLookup);

	freeConnectedTiles:
	for(x=0; x<map->width; x++)
		free(connectedTiles[x]);
	free(connectedTiles);

	return result;
}

struct BasePfNode*** createPfBase( enum terrainType **tiles, unsigned int width, unsigned int height ) {
	struct BasePfNode ***nodes = (struct BasePfNode***)calloc( width, sizeof(struct BasePfNode**));

	unsigned int x,y,dir;
	
	for(x=0; x<width; x++) {
		nodes[x] = (struct BasePfNode**)calloc( height, sizeof(struct BasePfNode*));

		for(y=0; y<height; y++) {
			struct BasePfNode *node = (struct BasePfNode*)malloc( sizeof(struct BasePfNode));

			node->pos.i = x;
			node->pos.j = y;

			for(dir=0; dir<4; dir++)
				node->neighbours[dir] = NULL;

			nodes[x][y] = node;
		}
	}

	struct Vector curPos, dirPos;
	struct BasePfNode *curNode, *dirNode;
	for(x=0; x<width; x++) {
		for(y=0; y<height; y++) {
			if( tiles[x][y] == terrain_gnd) {
				curPos.i = x;
				curPos.j = y;
				curNode = nodes[x][y];

				for( dir=1; dir<3; dir++) {
					vectorAdd( &dirPos, &curPos, &dirVectors[dir]);

					if( tiles[dirPos.i][dirPos.j] == terrain_gnd) {
						dirNode = nodes[dirPos.i][dirPos.j];
						curNode->neighbours[dir] = dirNode;
						dirNode->neighbours[DIR_REVERSE(dir)] = curNode;
					}
				}
			}
		}
	}

	return nodes;
}

void freeMap( struct Map *map) {
	int i,j;
	for( i=0; i<map->width; i++) {
		free( map->tiles[i]);
		free( map->objs[i]);
		for( j=0; j<map->height; j++) {
			free( map->pfBase[i][j]);
		}
		free( map->pfBase[i]);
	}
	free(map->tiles);
	free(map->objs);
	free(map->pfBase);

	for( i=0; i<map->objListCount; i++) {
		objectFree( map->objList[i]);
	}
	free( map->objList);

	free(map);
}
