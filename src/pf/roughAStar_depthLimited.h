#include "roughAStar.h"
#include "../aiFun.h"

bool roughAStar_dl_pathfind( struct RoughAStarData *data, struct Vector *fromPos, enum direction fromDir, struct Vector *toPos, int maxDepth, moveFun **result, int *resultCount);
