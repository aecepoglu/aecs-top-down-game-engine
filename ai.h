#ifndef AI_H
#define AI_H

#include "basic.h"
#include "object.h"
#include "map.h"

/* Forward declare 'struct object' */
struct object;

typedef void (aiUpdateFun)(struct Map*, struct object*, void*);
typedef struct AI* (aiCreateFun)();

enum aiType { ai_none, ai_leftTurner, };

struct AI {
	enum aiType type;
	void *data;
	aiUpdateFun *update;
};



/* Creates and returns an AI of given type.
	Returns null if there is no such AI */
struct AI* createAI( enum aiType); 

/*AI-utility functions*/
bool moveForward	( struct map*, struct object*);
bool moveBackward	( struct map*, struct object*);
bool turnLeft		( struct map*, struct object*);
bool turnRight		( struct map*, struct object*);


#endif
