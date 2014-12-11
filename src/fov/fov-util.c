#include "fov.h"
#include <stdlib.h>
#include <math.h>

#include "../log.h"

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

                printf("FOV_BASE %d,%d, %f, %f\n", n->pos.i, n->pos.j, n->maxLit[0], n->maxLit[1]);
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

void getFovObjects( struct Map *map, struct Vector *pos, enum terrainType **tiles, int range, struct ViewObject *objsSeen, int *objsSeenCount) {
	*objsSeenCount = 0;

	int i,j;
	struct Vector originPos = { pos->i - range, pos->j - range};
	struct Vector mapPos;

	for(i=0; i<=2*range; i++)
		for( j=0; j<=2*range; j++)
			if( /*i>=0 && j>=0 && i<map->width && j<map->height &&*/ tiles[i][j] >= terrain_gnd) {
				struct FOVBase *base = &fovBase[i][j];

				mapPos.i = originPos.i + i;
				mapPos.j = originPos.j + j;

				if( mapPos.i>=0 && mapPos.j >= 0 
					&& mapPos.i<map->width && mapPos.j<map->height 
					&& map->objs[ mapPos.i][ mapPos.j] != NULL)
				{
					struct ViewObject *vo = &objsSeen[ *objsSeenCount];

					vo->obj =  map->objs[ mapPos.i][ mapPos.j];
					vo->isFullySeen = base->lowerLimVisible && base->upperLimVisible;
					vo->pos.i = i;
					vo->pos.j = j;

					*objsSeenCount = *objsSeenCount +1;
				}
			}
}
