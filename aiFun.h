#ifndef AIFUN_H
#define AIFUN_H

#include "ai.h"
#include "object.h"
#include "map.h"

/*AI-utility functions*/
typedef void (aiUpdateFun)(struct Map*, struct object*, void*);
typedef struct AI* (aiCreateFun)();

bool moveForward	( struct Map*, struct object*);
bool moveBackward	( struct Map*, struct object*);
bool turnLeft		( struct Map*, struct object*);
bool turnRight		( struct Map*, struct object*);

#endif
