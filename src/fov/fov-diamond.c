#include "fov.h"
#include "../util/log.h"
#include "../definitions.h"

void fov_diamond(  struct Map *map, struct Vector *pos, enum direction dir, int range, struct ViewObject **tiles, struct TextureSheet **sprites) {
	log2("fov_diamond\n");
	int i,j;
	int mapX, mapY;
	int distance;

	bool tileSeen = false;
	for( i=0; i<VIEW_BOX_LENGTH; i++) {
		for( j=0; j<VIEW_BOX_LENGTH; j++) {
			distance = abs( VIEW_RANGE - i) + abs( VIEW_RANGE - j);
			mapX = i + pos->i - VIEW_RANGE;
			mapY = j + pos->j - VIEW_RANGE;

			if( distance <= range && mapX >= 0 && mapY >= 0 && mapX < map->width && mapY < map->height) {
				tiles[i][j].terrain = map->tiles[mapX][mapY];
				tileSeen = true;

				if( map->objs[ mapX][ mapY] != NULL) {
					setFovViewObject(
						&tiles[i][j],
						map->objs[mapX][mapY],
						true, mapX, mapY, sprites
					);
				}
			}
			else {
				tiles[i][j].terrain = terrain_dark;
				tileSeen = false;
			}

			fovBase[i][j].lowerLimVisible = tileSeen;
			fovBase[i][j].upperLimVisible = tileSeen;
		}
	}

	return;
}
