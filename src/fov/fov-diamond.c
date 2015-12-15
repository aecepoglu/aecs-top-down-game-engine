#include "fov.h"
#include "../util/log.h"
#include "../definitions.h"

void fov_diamond(  struct Map *map, struct Vector *pos, enum direction dir, int range, enum terrainType **tiles, struct TextureSheet **sprites, struct ViewObject *objsSeen, int *objsSeenCount) {
	log2("fov_diamond\n");
	int i,j;
	int mapX, mapY;
	int distance;

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
					setFovViewObject(
						&(objsSeen[*objsSeenCount]),
						map->objs[mapX][mapY],
						true, i, j, mapX, mapY, sprites
					);

					*objsSeenCount = *objsSeenCount + 1;
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
