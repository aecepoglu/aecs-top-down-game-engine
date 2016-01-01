#include "fov.h"
#include <stdlib.h>
#include <math.h>

#include "../util/log.h"

float fmodf_simple( float x, float mod) {
	if( x < 0)
		return x + mod;
	else if( x >= mod)
		return x - mod;
	else
		return x;
}

struct FOVBase** init_fovBase( int range) {

	struct FOVBase **result = (struct FOVBase**)calloc( 2*range + 1, sizeof( struct FOVBase*));

	float TWO_PI = 2* M_PI;

	int i,j;
	for( i=0; i<=2*range; i++)
		result[i] = (struct FOVBase*)calloc( 2*range + 1, sizeof( struct FOVBase));

	for( i=0; i<=2*range; i++) {
		for( j=0; j<=2*range; j++) {
			struct FOVBase *n = &result[i][j];
			n->lowerLimVisible = false;
			n->upperLimVisible = false;
			
			n->pos.i = i - range;
			n->pos.j = j - range;

			n->distance = (uint8_t)(abs( n->pos.i) + abs( n->pos.j));
			if( n->distance != 0) {
				int sliceIndex;
				if( n->pos.j >= 0) {
					sliceIndex = n->distance - n->pos.i;
				}
				else {
					sliceIndex = 3*n->distance + n->pos.i;
				}
				float slice = M_PI_4 / n->distance;
				n->maxLit[0] = fmodf_simple( (2*sliceIndex - 1)*slice, TWO_PI);
				n->maxLit[1] = fmodf_simple( (2*sliceIndex + 1)*slice, TWO_PI);
			}
			else {
				n->lit[0] = 0;
				n->lit[1] = TWO_PI;
			}

			n->neighbours[ dir_left] 	= n->pos.i<=0 && i>0 			? &result[i-1][j] : NULL;
			n->neighbours[ dir_right] 	= n->pos.i>=0 && i<2*range 	? &result[i+1][j] : NULL;
			n->neighbours[ dir_up] 		= n->pos.j<=0 && j>0 			? &result[i][j-1] : NULL;
			n->neighbours[ dir_down] 	= n->pos.j>=0 && j<2*range 	? &result[i][j+1] : NULL;
		}
	}

	fovBase = result; /*TODO this is just a tmp solution.
		In the end every agent should remember his fovBase. I might be able to use this for optimization too (taking the diff between what agent has seen before and what he's seeing now, maybe)
		Now I keep a fovBase defined in 'fov.h'
		It is built for the largest range possible.
	*/

	log2( "init_fovBase over\n");
	return result;
}

void free_fovBase( struct FOVBase **fovBase) {
	int i;
	for( i=0; i<sizeof(fovBase)/sizeof(struct FOVBase*); i++) {
		free( fovBase[i]);
	}
}

void setFovViewObject(struct ViewObject *vo, struct object *obj, bool isFullySeen, int worldX, int worldY, struct TextureSheet **sprites) {
	vo->obj = obj;
	vo->isFullySeen = isFullySeen;
	struct TextureSheet *ts = sprites[obj->textureId];

	float unitWidth =
		(float)ts->spriteWidth / (float)obj->width;
	float unitHeight =
		(float)ts->spriteHeight / (float)(obj->height + ts->tallness);

	vo->srcRect.w = unitWidth;
	vo->srcRect.h = unitHeight * (ts->tallness + 1);
	vo->srcRect.x = (worldX - obj->pos.i) * unitWidth;
	vo->srcRect.y = (worldY - obj->pos.j) * unitHeight;
}

void getFovObjects (struct Map *map, struct Vector *pos, struct ViewObject **tiles, int range, struct TextureSheet **sprites) {
	struct Vector originPos = { pos->i - range, pos->j - range};
	struct Vector mapPos;

	int i, j;
	for (i=0; i <= 2*range; i++)
		for (j=0; j <= 2*range; j++)
			if (tiles[i][j].terrain >= terrain_gnd) {
				struct FOVBase *base = &fovBase[i][j];

				mapPos.i = originPos.i + i;
				mapPos.j = originPos.j + j;

				if (mapPos.i >= 0 &&
				    mapPos.j >= 0 &&
				    mapPos.i < map->width &&
				    mapPos.j < map->height &&
				    map->objs[ mapPos.i][ mapPos.j] != NULL) {

					setFovViewObject( &tiles[i][j],
					                  map->objs[ mapPos.i ][ mapPos.j ],
					                  base->lowerLimVisible && base->upperLimVisible,
					                  mapPos.i, mapPos.j, sprites
					);
				}
			}
}

void clearFovObjects (struct ViewObject **tiles, int range) {
	int i, j;
	for (i=0; i <= 2 * range; i ++) {
		for (j=0; j <= 2 * range; j ++) {
			tiles[i][j].obj = NULL;
		}
	}
}
