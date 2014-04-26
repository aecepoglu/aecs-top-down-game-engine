#ifndef AI_H
#define AI_H

#include "basic.h"
#include "object.h"
#include "map.h"


enum aiType { ai_none, ai_leftTurner, ai_hungryLeftTurner};

struct AI {
	enum aiType type;
	void *data;
};


#endif
