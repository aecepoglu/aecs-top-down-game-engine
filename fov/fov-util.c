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
				float angle = (M_PI_2 / n->distance )* sliceIndex;
				n->maxLit[0] = fmodf_simple( angle - slice, TWO_PI);
				n->maxLit[1] = fmodf_simple( angle + slice, TWO_PI);
			}
			else {
				n->lit[0] = 0;
				n->lit[1] = TWO_PI;
			}

			n->neighbours[ dir_left] 	= n->pos.i<=0 && i>0 			? &result[i-1][j] : NULL;
			n->neighbours[ dir_right] 	= n->pos.i>=0 && i<= 2*range 	? &result[i+1][j] : NULL;
			n->neighbours[ dir_up] 		= n->pos.j<=0 && j>0 			? &result[i][j-1] : NULL;
			n->neighbours[ dir_down] 	= n->pos.j>=0 && j<=2*range 	? &result[i][j+1] : NULL;
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
