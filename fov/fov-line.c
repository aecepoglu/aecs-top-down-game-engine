#include "fov.h"
#include "../log.h"

void fov_line(  struct Map *map, struct Vector *pos, enum direction dir, int range, enum terrainType **tiles, struct ViewObject *objsSeen, int *objsSeenCount) {
	log2("fov_line\n");
	int i,j;
	for( i=0; i<2*range + 1; i++) {
		for( j=0; j<2*range + 1; j++) {
			tiles[i][j] = terrain_dark;
		}
	}
	log3("reset tiles\n");

	*objsSeenCount=0;

	struct Vector tilesPos = { range, range};
	struct Vector mapPos;
	vectorClone( &mapPos, pos);
	struct Vector *dPos = & dirVectors[ dir];

	bool sawSelf = false;

	while( range >= 0) {

		log3("\tpos (%d,%d)-(%d,%d)\n", tilesPos.i, tilesPos.j, mapPos.i, mapPos.j);
		tiles[ tilesPos.i][ tilesPos.j] = map->tiles[ mapPos.i][ mapPos.j];
		if( map->tiles[ mapPos.i][ mapPos.j] == terrain_wall) {
			break;
		}
		else if( map->objs[ mapPos.i][ mapPos.j] != NULL) {
			struct ViewObject *vo = &objsSeen[ *objsSeenCount];
			vo->obj = map->objs[ mapPos.i][ mapPos.j];
			vo->isFullySeen = true;
			vectorClone( &vo->pos, &tilesPos);

			*objsSeenCount = *objsSeenCount + 1;
			if( ! sawSelf )
				sawSelf = true;
			else
				break;
		}

		vectorAdd( &tilesPos, &tilesPos, dPos);
		vectorAdd( &mapPos, &mapPos, dPos);
		range --;
	}

	return;
}
