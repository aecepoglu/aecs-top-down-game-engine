#include "fov.h"
#include "../log.h"
#include "../queue.h"
#include "../definitions.h"
#include <math.h>

#define QUEUE_SIZE VIEW_BOX_PERIMETER
struct FOVBase *queue[ QUEUE_SIZE];
int queueHead, queueTail;
const float TWO_PI = M_PI * 2;

bool fov_inAngleLimits( float angle, float min, float max) {
	if( max < min)
		return !((angle < min) && (angle > max));
	else
		return angle>=min && angle <= max;
}

void fov_castLight( struct FOVBase *from, struct FOVBase *to) {
	to->lit[0] = fov_inAngleLimits( from->lit[0], to->maxLit[0], to->maxLit[1]) 
		? from->lit[0]
		: to->maxLit[0];
	to->lit[1] = fov_inAngleLimits( from->lit[1], to->maxLit[0], to->maxLit[1]) 
		? from->lit[1]
		: to->maxLit[1];
}

void fov_raycast( struct Map *map, struct Vector *pos, enum direction objDir, int range, enum terrainType **tiles, struct ViewObject *objsSeen, int *objsSeenCount) {
	log2("fov_raycast from pos (%d,%d) with dir %d\n", pos->i, pos->j, objDir);

	int i,j,dir;

	// clear
	for( i=0; i<=2*range; i++)
		for( j=0; j<=2*range; j++) {
			tiles[i][j] = terrain_dark;
			fovBase[i][j].lowerLimVisible = false;
			fovBase[i][j].upperLimVisible = false;
		}

	log3("cleared\n");
	
	struct Vector centerPos = { range, range};
	struct Vector tilePos, mapPos;
	
	struct FOVBase *node = &fovBase[ centerPos.i][ centerPos.j];

	bool grows[4] = { true, true, false, true };
	for( dir=0; dir<4; dir++)
		node->grows[  (objDir + dir) % 4 ] = grows[ dir];

	queueHead = 0;
	queueTail = 0;
	*objsSeenCount = 0;

	node->lowerLimVisible= true;
	node->upperLimVisible= true;
	QUEUE_PUSH( queue, node, queueHead, queueTail, QUEUE_SIZE);

	struct FOVBase *neighbourNode;
	while( QUEUE_IS_EMPTY( queueHead, queueTail) != true) { //TODO enters the init node twice? Test this
		QUEUE_POP( queue, node, queueHead, queueTail, QUEUE_SIZE);

		log3( "Expanding node %d,%d, lowVis: %d, upVis: %d\n", node->pos.i, node->pos.j, node->lowerLimVisible, node->upperLimVisible);

		vectorAdd( &mapPos, pos, &node->pos);
		vectorAdd( &tilePos, &centerPos, &node->pos);
		tiles[ tilePos.i][ tilePos.j] = map->tiles[ mapPos.i][ mapPos.j];

		if( map->objs[ mapPos.i][ mapPos.j] != NULL) {
			struct ViewObject *vo = &objsSeen[ *objsSeenCount];
			vo->isFullySeen = node->lowerLimVisible && node->upperLimVisible;
			vo->obj = map->objs[ mapPos.i][ mapPos.j];
			vectorClone( &vo->pos, &tilePos);
			*objsSeenCount = *objsSeenCount + 1;
		}

		if( map->tiles[ mapPos.i][ mapPos.j] != terrain_wall) {

			for( dir=0; dir<4; dir++) {
				if( node->grows[ dir]) {
					neighbourNode = node->neighbours[ dir];
					log3(" Checking neighbour at dir %d: %d,%d, lowVis: %d, upVis: %d\n", dir, neighbourNode->pos.i, neighbourNode->pos.j, neighbourNode->lowerLimVisible, neighbourNode->upperLimVisible);
					if( (neighbourNode->lowerLimVisible && neighbourNode->upperLimVisible) != true && neighbourNode->distance < range) {

						if( neighbourNode->lowerLimVisible) {
							bool upperLimVisible = fov_inAngleLimits( neighbourNode->maxLit[1], node->lit[0], node->lit[1]);

							if( upperLimVisible) {
								neighbourNode->upperLimVisible = true;

								neighbourNode->lit[0] = neighbourNode->maxLit[0];
								neighbourNode->lit[1] = neighbourNode->maxLit[1];
							}
						}
						else if( neighbourNode->upperLimVisible) {
							bool lowerLimVisible = fov_inAngleLimits( neighbourNode->maxLit[0], node->lit[0], node->lit[1]);

							if( lowerLimVisible) {
								neighbourNode->lowerLimVisible = true;

								neighbourNode->lit[0] = neighbourNode->maxLit[0];
								neighbourNode->lit[1] = neighbourNode->maxLit[1];
							}
						}
						else {
							bool lowerLimVisible = fov_inAngleLimits( neighbourNode->maxLit[0], node->lit[0], node->lit[1]);
							bool upperLimVisible = fov_inAngleLimits( neighbourNode->maxLit[1], node->lit[0], node->lit[1]);
							log3("  lowerLimVisible: %d\n  upperLimVisible: %d\n", lowerLimVisible, upperLimVisible);
							neighbourNode->lowerLimVisible = lowerLimVisible;
							neighbourNode->upperLimVisible = upperLimVisible;

							if( lowerLimVisible || upperLimVisible) {
								if( lowerLimVisible && upperLimVisible) {
									neighbourNode->lit[0] = neighbourNode->maxLit[0];
									neighbourNode->lit[1] = neighbourNode->maxLit[1];
								}
								else if( lowerLimVisible) {
									neighbourNode->lit[0] = neighbourNode->maxLit[0];
									neighbourNode->lit[1] = node->lit[1];
								}
								else if( upperLimVisible) {
									neighbourNode->lit[0] = node->lit[0];
									neighbourNode->lit[1] = neighbourNode->maxLit[1];
								}
	
								QUEUE_PUSH( queue, neighbourNode, queueHead, queueTail, QUEUE_SIZE);
								
								//TODO pre-calculate *grows* and use that.
								neighbourNode->grows[ dir] = true;
								neighbourNode->grows[ DIR_REVERSE( dir)] = false;
								int leftDir = DIR_ROTATE_LEFT( dir);
								int rightDir = DIR_ROTATE_RIGHT( dir);
								neighbourNode->grows[ leftDir] = node->grows[ leftDir];
								neighbourNode->grows[ rightDir] = node->grows[ rightDir];
							}
						}
					}
				}
			}
		}
	}
	
	log2("fov_raycast end\n");
}
