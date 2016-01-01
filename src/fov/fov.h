#ifndef FOV_H
#define FOV_H

#include <SDL.h>
#include "../core/map.h"
#include "../vector.h"
#include "../core/texture.h"

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
	enum terrainType terrain;
	SDL_Rect srcRect;
	bool isFullySeen;
};

struct FOVBase **fovBase;

float fmodf_simple( float angle, float mod);
struct FOVBase** init_fovBase( int range);
void free_fovBase( struct FOVBase **fovBase);
void getFovObjects( struct Map *map, struct Vector *pos, struct ViewObject **tiles, int tilesLen, struct TextureSheet **objSprites);
void setFovViewObject( struct ViewObject *result, struct object *objSeen, bool isFullySeen, int worldX, int worldY, struct TextureSheet **objSprites );
void clearFovObjects (struct ViewObject **tiles, int range);


/*
	FOV FUNCTIONS
	=============
*/

typedef void (fovFun)( struct Map *map, struct Vector *pos, enum direction dir, int range, struct ViewObject **tiles, struct TextureSheet **sprites );

/* FOV covered by a single ray going towards the $dir direction
*/
fovFun fov_line;

/* Ray casting half a circle.
	Penetrating through objects but not walls
*/
fovFun fov_raycast;

/* Diamond shape.
	Sees through walls and objects
*/
fovFun fov_diamond;

#endif /* FOV_H */
