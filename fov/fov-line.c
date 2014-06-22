#include "fov.h"
#include "../log.h"

void fov_line(  struct Map *map, struct Vector *pos, enum direction dir, enum terrainType **tiles, int range) {
	log2("fov_line\n");
	int i,j;
	for( i=0; i<2*range + 1; i++) {
		for( j=0; j<2*range + 1; j++) {
			tiles[i][j] = terrain_dark;
		}
	}
	log3("reset tiles\n");

	struct Vector tilesPos = { range, range};
	struct Vector mapPos;
	vectorClone( &mapPos, pos);
	struct Vector *dPos = & dirVectors[ dir];

	bool endNextTurn = false;
	do {
		if( map->tiles[ mapPos.i][ mapPos.j] == terrain_wall)
			endNextTurn = true;

		log3("\tpos (%d,%d)-(%d,%d)\n", tilesPos.i, tilesPos.j, mapPos.i, mapPos.j);
		tiles[ tilesPos.i][ tilesPos.j] = map->tiles[ mapPos.i][ mapPos.j];
		vectorAdd( &tilesPos, &tilesPos, dPos);
		vectorAdd( &mapPos, &mapPos, dPos);
		range --;
		/*
		if( map->objs[ node->pos.i][ node->pos.j] != NULL) {
			
		}
		*/
	}
	while( range >= 0 && endNextTurn != true);

	return;
}
