#ifndef FOV_H

#include "../map.h"
#include "../vector.h"

struct FOVBase {
	struct Vector pos;
	uint8_t distance;
	struct FOVBase *neighbours[4];
	bool grows[4];

	bool lowerLimVisible, upperLimVisible;
	float lit[2];
	float maxLit[2];
};

struct ViewObject {
	struct object *obj;
	struct Vector pos;
	bool isFullySeen;
};

struct FOVBase **fovBase;

float fmodf_simple( float angle, float mod);
struct FOVBase** init_fovBase( int range);

/*
	FOV FUNCTIONS
	=============
*/

typedef void (fovFun)( struct Map *map, struct Vector *pos, enum direction dir, int range, enum terrainType **tiles, struct ViewObject *objsSeen, int *objsSeenCount);

/* FOV covered by a single ray going towards the $dir direction
*/
fovFun fov_line;

/* FOV covered by a half circle
	Sees through walls and objects
*/
fovFun fov_rough;

/* Ray casting half a circle.
	Penetrating through objects but not walls
*/
fovFun fov_raycast;

#endif /* FOV_H */
