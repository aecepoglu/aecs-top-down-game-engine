#include "fov.h"
#include "../log.h"

#include <assert.h>

struct FOVBase *queue[16];
int queueCount;

void fov_rough( struct Map *map, struct Vector *pos, enum direction objDir, enum terrainType **tiles, int range) {
	log2("fov_rough from pos (%d,%d) with dir %d\n", pos->i, pos->j, objDir);

	int i,j,dir;

	// clear
	for( i=0; i<=2*range; i++)
		for( j=0; j<=2*range; j++)
			tiles[i][j] = terrain_dark;

	log3("cleared\n");
	
	struct Vector centerPos = { range, range};
	struct Vector tilePos, mapPos;
	
	struct FOVBase *node = &fovBase[ centerPos.i][ centerPos.j];

	bool grows[4] = { true, true, false, true };
	for( dir=0; dir<4; dir++)
		node->grows[  (objDir + dir) % 4 ] = grows[ dir];

	queueCount = 0;
	queue[ queueCount] = node;
	queueCount ++;

	struct FOVBase *neighbourNode;
	while( queueCount > 0) {
		queueCount --;
		node = queue[ queueCount];

		log2("\tnode: %d,%d\n", node->pos.i, node->pos.j);
		vectorAdd( &mapPos, pos, &node->pos);
		log2("\tmap pos %d,%d\n", mapPos.i, mapPos.j);
		if( mapPos.i < 0 || mapPos.i == map->width || mapPos.j < 0 || mapPos.j == map->height )
			continue;
		vectorAdd( &tilePos, &centerPos, &node->pos);
		log2("\ttile pos %d,%d\n", tilePos.i, tilePos.j);
		tiles[ tilePos.i][ tilePos.j] = map->tiles[ mapPos.i][ mapPos.j];

		for( dir=0; dir<4; dir++) {
			neighbourNode = node->neighbours[ dir];
			
			if( node->grows[dir] == true && neighbourNode->visited != true && neighbourNode->distance < range ) {
				neighbourNode->visited = true;
				
				neighbourNode->grows[ dir] = true;
				neighbourNode->grows[ DIR_REVERSE( dir)] = false;
			    int leftDir = DIR_ROTATE_LEFT( dir);
			    int rightDir = DIR_ROTATE_RIGHT( dir);
				neighbourNode->grows[ leftDir] = node->grows[ leftDir];
				neighbourNode->grows[ rightDir] = node->grows[ rightDir];

				queue[ queueCount] = neighbourNode;
				queueCount ++;
			}
		}
	}

	for( i=0; i<=2*range; i++)
		for( j=0; j<=2*range; j++)
			fovBase[i][j].visited = false;

	log2("fov_rough end\n");
}
