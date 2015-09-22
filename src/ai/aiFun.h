#ifndef AIFUN_H
#define AIFUN_H

#include "ai.h"
#include "../core/object.h"
#include "../core/map.h"
#include "../util/log.h"

/*AI-utility functions*/
typedef void (aiUpdateFun)(struct Map*, struct object*, void*);
typedef void (aiDestroyFun)(struct AI*);
typedef struct AI* (aiCreateFun)();
typedef void (aiSeenFun)(struct AI*);
typedef void (aiUseFun)(struct Map*, struct object*, void*);

typedef bool (moveFun)( struct Map *, struct object*);

moveFun moveForward;
moveFun moveBackword;
moveFun turnLeft;
moveFun turnRight;
moveFun eat;

#endif
