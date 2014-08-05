#ifndef FOV_H

#include "../map.h"
#include "../vector.h"

struct FOVBase {
	struct FOVBase *neighbours[4];
	bool lowerLimVisible, upperLimVisible;
	uint8_t distance;
	bool grows[4];
	struct Vector pos;

	float angle;
	float lit[2];
	float maxLit[2];
};

struct FOVBase **fovBase;

float fmodf_simple( float angle, float mod);
struct FOVBase** init_fovBase( int range);

/*
	FOV FUNCTIONS
	=============
*/

/* FOV covered by a single ray going towards the $dir direction
*/
void fov_line( struct Map *map, struct Vector *pos, enum direction dir, enum terrainType **tiles, int range);

/* FOV covered by a half circle
	Sees through walls and objects
*/
void fov_rough( struct Map *map, struct Vector *pos, enum direction dir, enum terrainType **tiles, int range);

/* Ray casting half a circle.
	Penetrating through objects but not walls
*/
void fov_raycast( struct Map *map, struct Vector *pos, enum direction dir, enum terrainType **tiles, int range);

#endif /* FOV_H */
