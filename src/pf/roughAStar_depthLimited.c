#include "roughAStar_depthLimited.h"
#include "../log.h"
#include "../stack.h"
#include <stdlib.h>

#define HEURISTICS( pos1, pos2) ( abs( (pos1).i - pos2->i) + abs( (pos1).j - pos2->j) + ((pos1).i != pos2->i) + ((pos1).j != pos2->j) )

void roughAStar_dl_constructPath( struct RoughPfNode ***map, struct RoughPfNode *first, struct RoughPfNode *last, moveFun** result, int *resultCount) {
	log2("constructPath from %d,%d dir %d, to %d,%d dir %d\n", first->base->pos.i, first->base->pos.j, first->cameDir, last->base->pos.i, last->base->pos.j, last->cameDir);
	int count = 0;
	
	struct Vector pos = last->base->neighbours[ DIR_REVERSE( last->cameDir)]->pos;
	struct RoughPfNode *cur = map[ pos.i][ pos.j];
	struct RoughPfNode *next = last;

	while(true) {
		log2("\t(%d,%d (%d))\n", pos.i, pos.j, cur->cameDir);

		STACK_PUSH( result, moveForward, count);

		int diff = (cur->cameDir - next->cameDir) %4;
		log2("\t\tdiff: %d\n", diff);
		switch( diff) {
			case 1:
				STACK_PUSH( result, turnLeft, count);
				break;
			case 2:
				STACK_PUSH( result, turnLeft, count);
				STACK_PUSH( result, turnLeft, count);
				break;
			case 3:
				STACK_PUSH( result, turnRight, count);
				break;
		};
		
		if( cur == first)
			break;

		pos = cur->base->neighbours[ DIR_REVERSE(cur->cameDir)]->pos;
		next = cur;
		cur = map[ pos.i][ pos.j];
	}
	
	log2("found path is %d long\n", count);
	*resultCount = count;
}

bool roughAStar_dl_pathfind( struct RoughAStarData *data, struct Vector *fromPos, enum direction fromDir, struct Vector *toPos, int maxDepth, moveFun **result, int *resultCount) {
	log1("rough A*, pathfind from %d,%d to %d,%d\n", fromPos->i, fromPos->j, toPos->i, toPos->j);

	if( vectorEquals( fromPos, toPos))
		return false;

	//clear the open and closed nodes sets
	struct LinkedListNode *setsToClear[2] = { data->openSet, data->closedSet};
	int i;
	for( i=0; i<2; i++) {
		struct LinkedListNode *listNode, *nextListNode;
		listNode = setsToClear[i];
		while( listNode) {
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
	node->fValue = HEURISTICS( *fromPos, toPos);

	linkedList_push( &data->openSet, node);

	bool isFirstIteration = true;
	int openedCount=0;

	while( data->openSet) {
		openedCount ++;

		/* Find node with minimum fValue in data->openSet
            _node_ will be the RoughPfNode with min fValue
            _minListNode_ will be the LinkedListNode whose _data_ is _node_
        */
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

        /* Found node */

		log2("\tExpanding node %d,%d. fValue=%d\n", node->base->pos.i, node->base->pos.j, node->fValue);

		if( vectorEquals( & node->base->pos, toPos) || openedCount > maxDepth) {
			//Create a path from fromPos to toPos and return it
			log2("\t\tFound goal. Create the path and return it\n");
			roughAStar_dl_constructPath( data->map, data->map[ fromPos->i][ fromPos->j], node, result, resultCount);
			return true;
		}

		linkedList_remove( &data->openSet, minListNode);
		free( minListNode);
		linkedList_push( &data->closedSet, node);

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
						linkedList_push( &data->openSet, neighbourNode);
				}
			}
		}

		isFirstIteration = false;
	}
	return false;
}
#undef HEURISTICS
