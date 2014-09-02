#ifndef AIFUN_H
#define AIFUN_H

#include "ai.h"
#include "object.h"
#include "map.h"
#include "log.h"

/*AI-utility functions*/
typedef void (aiUpdateFun)(struct Map*, struct object*, void*);
typedef void (aiDestroyFun)(struct AI*);
typedef struct AI* (aiCreateFun)();
typedef void (aiSeenFun)(struct AI*);

typedef bool (moveFun)( struct Map *, struct object*);

moveFun moveForward;
moveFun moveBackword;
moveFun turnLeft;
moveFun turnRight;
moveFun eat;

#endif
