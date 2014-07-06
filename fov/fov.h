#ifndef FOV_H

#include "../map.h"
#include "../vector.h"

struct FOVBase {
	struct FOVBase *neighbours[4];
	bool visited;
	uint8_t distance;
	bool grows[4];
	struct Vector pos;
};

struct FOVBase **fovBase;


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

#endif /* FOV_H */
