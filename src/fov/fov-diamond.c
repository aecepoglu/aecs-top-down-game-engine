#include "fov.h"
#include "../util/log.h"
#include "../definitions.h"

void fov_diamond(  struct Map *map, struct Vector *pos, enum direction dir, int range, enum terrainType **tiles, struct ViewObject *objsSeen, int *objsSeenCount) {
	log2("fov_diamond\n");
	int i,j;
	int mapX, mapY;
	int distance;
	struct ViewObject *vo;

	*objsSeenCount=0;
	bool tileSeen = false;
	for( i=0; i<VIEW_BOX_LENGTH; i++) {
		for( j=0; j<VIEW_BOX_LENGTH; j++) {
			distance = abs( VIEW_RANGE - i) + abs( VIEW_RANGE - j);
			mapX = i + pos->i - VIEW_RANGE;
			mapY = j + pos->j - VIEW_RANGE;

			if( distance <= range && mapX >= 0 && mapY >= 0 && mapX < map->width && mapY < map->height) {
				tiles[i][j] = map->tiles[mapX][mapY];
				tileSeen = true;

				if( map->objs[ mapX][ mapY] != NULL) {
					vo = &(objsSeen[*objsSeenCount]);
					*objsSeenCount = *objsSeenCount + 1;

					vo->obj = map->objs[ mapX][ mapY];
					vo->isFullySeen = true;
					vo->pos.i = i;
					vo->pos.j = j;
				}
			}
			else {
				tiles[i][j] = terrain_dark;
				tileSeen = false;
			}

			fovBase[i][j].lowerLimVisible = tileSeen;
			fovBase[i][j].upperLimVisible = tileSeen;
		}
	}

	return;
}
