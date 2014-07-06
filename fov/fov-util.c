#include "fov.h"
#include <stdlib.h>

#include "../log.h"

struct FOVBase** init_fovBase( int range) {
	
	struct FOVBase **result = (struct FOVBase**)calloc( 2*range + 1, sizeof( struct FOVBase*));

	int i,j;
	for( i=0; i<=2*range; i++)
		result[i] = (struct FOVBase*)calloc( 2*range + 1, sizeof( struct FOVBase));

	for( i=0; i<=2*range; i++) {
		for( j=0; j<=2*range; j++) {
			struct FOVBase *n = &result[i][j];
			n->visited = false;
			
			n->pos.i = i - range;
			n->pos.j = j - range;

			n->distance = (uint8_t)(abs( n->pos.i) + abs( n->pos.j));

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
