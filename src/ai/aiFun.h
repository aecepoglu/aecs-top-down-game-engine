#ifndef AIFUN_H
#define AIFUN_H

#include "ai.h"
#include "../core/object.h"
#include "../core/map.h"
#include "../util/log.h"
#include "../move.h"

/*AI-utility functions*/
typedef void (aiUpdateFun)(struct Map*, struct object*, void*);
typedef void (aiDestroyFun)(struct AI*);
typedef struct AI* (aiCreateFun)();
typedef void (aiSeenFun)(struct AI*);
typedef void (aiUseFun)(struct Map*, struct object*, void*);

#endif
