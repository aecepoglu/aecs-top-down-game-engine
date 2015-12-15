#include "fov.h"
#include "../util/log.h"
#include "../definitions.h"

//FIXME use setFovViewObject(...) in here
void fov_line(  struct Map *map, struct Vector *pos, enum direction dir, int range, enum terrainType **tiles, struct TextureSheet **sprites, struct ViewObject *objsSeen, int *objsSeenCount) {
	log2("fov_line\n");
	int i,j;
	for( i=0; i<VIEW_BOX_LENGTH; i++) {
		for( j=0; j<VIEW_BOX_LENGTH; j++) {
			tiles[i][j] = terrain_dark;
			fovBase[i][j].lowerLimVisible = false;
			fovBase[i][j].upperLimVisible = false;
		}
	}
	log3("reset tiles\n");

	*objsSeenCount=0;

	struct Vector tilesPos = { VIEW_RANGE, VIEW_RANGE};
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
			fovBase[ tilesPos.i][ tilesPos.j].lowerLimVisible = true;
			fovBase[ tilesPos.i][ tilesPos.j].upperLimVisible = true;
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
