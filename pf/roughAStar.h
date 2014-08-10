#include "../map.h"
#include "../vector.h"
#include "../linkedList.h"

struct RoughPfNode {
	int gValue;
	int fValue;
	struct BasePfNode *base;
	enum direction cameDir; //the direction we're facing after visiting this node
};

struct RoughAStarData {
	struct LinkedListNode *openSet, *closedSet;
	struct RoughPfNode ***map;
};

struct RoughAStarData* roughAStar_initData( struct Map *map);

void roughAStarData_free( struct RoughAStarData *data);

void* roughAStar_pathfind( struct RoughAStarData *data, struct Vector *fromPos, enum direction fromDir, struct Vector *toPos);

void* constructPath( struct RoughPfNode ***map, struct RoughPfNode *first, struct RoughPfNode *last);
