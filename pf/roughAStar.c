#include <stdlib.h>
#include "roughAStar.h"

#include "../aiFun.h"


#define HEURISTICS( pos1, pos2) ( abs( pos1.i - pos2->i) + abs( pos1.j - pos2->j) + (pos1.i != pos2->i) + (pos1.j != pos2->j) )

struct RoughAStarData* roughAStar_initData( struct Map *map) {
	struct RoughAStarData *result = (struct RoughAStarData*)malloc( sizeof( struct RoughAStarData));

	result->openSet = NULL;
	result->closedSet = NULL;

	int x,y;
	struct RoughPfNode ***m = (struct RoughPfNode***)calloc( map->width, sizeof( struct RoughPfNode**));
	for( x=0; x<map->width; x++) {
		m[x] = (struct RoughPfNode**)calloc( map->width, sizeof( struct RoughPfNode*));
		for(y=0; y<map->height; y++) {
			struct RoughPfNode *n = (struct RoughPfNode*)malloc( sizeof( struct RoughPfNode));
			n->base = map->pfBase[x][y];
			m[x][y] = n;
			//TODO set node defaults if any
		}
	}

	result->map = m;

	return result;
}

void* roughAStar_pathfind( struct RoughAStarData *data, struct Vector *fromPos, enum direction fromDir, struct Vector *toPos) {
	log1("rough A*, pathfind from %d,%d to %d,%d\n", fromPos->i, fromPos->j, toPos->i, toPos->j);

	if( vectorEquals( fromPos, toPos))
		return NULL;

	//clear the open and closed nodes sets
	struct LinkedListNode *setsToClear[2] = { data->openSet, data->closedSet};
	int i;
	for( i=0; i<2; i++) {
		struct LinkedListNode *listNode, *nextListNode;
		listNode = setsToClear[i];
		while( listNode) {
			//struct RoughPfNode *node = (struct RoughPfNode*)listNode->data;
			//node->gValue = 0;
			//node->fValue = 0;
			//node->cameFrom = NULL;
			nextListNode = listNode->next;
			free( listNode);
			listNode = nextListNode;
		}
	}
	data->openSet = NULL;
	data->closedSet = NULL;

	log2("start searching\n");

	//starting node
	struct RoughPfNode *node = data->map[fromPos->i][fromPos->j];
	node->cameDir = fromDir;
	node->gValue = 0;
	node->fValue = HEURISTICS( node->base->pos, toPos);

	linkedList_add( &data->openSet, node);

	bool isFirstIteration = true;

	while( data->openSet) {
		//find the node with minimum f-value
		log2("\tfind node with min f value\n");
		struct LinkedListNode *listNode = data->openSet;
		struct LinkedListNode *minListNode = data->openSet;
		node = (struct RoughPfNode*)listNode->data;
		log2("\tOpenSet: { ");
		while( listNode != NULL) {
			struct RoughPfNode *tmpNode = (struct RoughPfNode*)(listNode->data);
			log2("[%d,%d %d] ", tmpNode->base->pos.i, tmpNode->base->pos.j, tmpNode->fValue);
			if( tmpNode->fValue < node->fValue) {
				minListNode = listNode;
				node = (struct RoughPfNode*)listNode->data;
			}
			listNode = listNode->next;
		}
		log2(" } \n");

		log2("\tExpanding node %d,%d. fValue=%d\n", node->base->pos.i, node->base->pos.j, node->fValue);

		if( vectorEquals( & node->base->pos, toPos)) {
			//Create a path from fromPos to toPos and return it
			log2("\t\tFound goal. Create the path and return it\n");
			return constructPath( data->map, data->map[ fromPos->i][ fromPos->j], node);
		}

		linkedList_remove( &data->openSet, minListNode);
		free( minListNode);
		linkedList_add( &data->closedSet, node);

		log2("\tcheck neighbours\n");

		int dir;
		int skipDir = DIR_REVERSE(node->cameDir);
		for( dir=0; dir<4; dir++) {
			if( dir == skipDir && !isFirstIteration )
				continue;

			struct BasePfNode *neighbourBase = node->base->neighbours[ dir];

			if( neighbourBase ) {
				struct RoughPfNode *neighbourNode = data->map[ neighbourBase->pos.i][ neighbourBase->pos.j];

				if( linkedList_find( data->closedSet, neighbourNode) != NULL ) 
					continue;

				int dirDiff = (node->cameDir - dir) % 4;
				if( dirDiff %2 != 0)
					dirDiff = 1;
				int gValue = node->gValue + 1 + dirDiff;
				bool neighbourNotInOpenSet = linkedList_find( data->openSet, neighbourNode) == NULL;

				if( neighbourNotInOpenSet || gValue < neighbourNode->gValue) {
					neighbourNode->gValue = gValue;
					neighbourNode->fValue = gValue + HEURISTICS( neighbourNode->base->pos, toPos);
					log2("\t\tneighbourNode(%d,%d) updates as gValue:%d fValue:%d\n", neighbourNode->base->pos.i, neighbourNode->base->pos.j, gValue, neighbourNode->fValue);
					neighbourNode->cameDir = dir;

					if( neighbourNotInOpenSet)
						linkedList_add( &data->openSet, neighbourNode);
				}
			}
		}

		isFirstIteration = false;
	}
	return NULL;
}

void* constructPath( struct RoughPfNode ***map, struct RoughPfNode *first, struct RoughPfNode *last) {
	log2("constructPath from %d,%d dir %d, to %d,%d dir %d\n", first->base->pos.i, first->base->pos.j, first->cameDir, last->base->pos.i, last->base->pos.j, last->cameDir);
	struct LinkedListNode *result = NULL;
	
	struct Vector pos = last->base->neighbours[ DIR_REVERSE( last->cameDir)]->pos;
	struct RoughPfNode *cur = map[ pos.i][ pos.j];
	struct RoughPfNode *next = last;
			//linkedList_add( &result, moveForward);

	while(true) {
		log2("\t(%d,%d (%d))\n", pos.i, pos.j, cur->cameDir);
		//if( cur != first)

		linkedList_add( &result, moveForward);

		int diff = (cur->cameDir - next->cameDir) %4;
		log2("\t\tdiff: %d\n", diff);
		switch( diff) {
			//case 0:
			//	linkedList_add( &result, moveForward);
			//	break;
			case 1:
				linkedList_add( &result, turnLeft);
				break;
			case 2:
				linkedList_add( &result, turnLeft);
				linkedList_add( &result, turnLeft);
				break;
			case 3:
				linkedList_add( &result, turnRight);
				break;
		};
		
		if( cur == first)
			break;

		pos = cur->base->neighbours[ DIR_REVERSE(cur->cameDir)]->pos;
		next = cur;
		cur = map[ pos.i][ pos.j];
	}

	return result;
}

#undef HEURISTICS
