#include "game.h"

#include "core/engine.h"
#include "ai/aiTable.h"
#include "fov/fov.h"
#include "definitions.h"
#include "textConsole.h"
#include "inventory.h"
#include "collection/array.h"
#include "dsl.h"
#include "core/audio.h"
#include "customEventTypes.h"
#include "cutscene.h"
#include "texture/spriteSpecs.h"
#include "util/util.h"

SDL_Event timerPushEvent = {
	.type= SDL_USEREVENT,
};
SDL_TimerID levelTimer;


/* The player is allowed to play only once per tick.
	This variable shows whether the player has moved or not */
bool playerMoved = false;

//FIXME The program will crash if the window is too small (test if this is still happening). Render window un-usable and show a notification message about it.

#define PLAYER_FOV_TILES_LIM VIEW_BOX_LENGTH
bool makeTransparentForGround = false;
int viewRange;
int vi_padding, vj_padding;
struct ViewObject **playerVisibleTiles;
struct SpriteSpecsList *spriteSpecs;
struct Vector mouseTile;
struct object *highlitObject;


const Uint32 timerDelay = 100 /*miliseconds*/;


#define CALL_FOV_FCN() clearFovObjects(playerVisibleTiles, VIEW_RANGE); currentFov(myMap, &player->pos, player->dir, viewRange, playerVisibleTiles, textures->obj)
#define PLAYER_DISTANCE( pos1) (abs( (pos1)->i - player->pos.i) + abs( (pos1)->j - player->pos.j))


void setViewRange( int value ) {
	viewRange = value;
}

void setViewPaddings() {
	vi_padding = viewSize.i/2 - VIEW_RANGE;
	vj_padding = viewSize.j/2 - VIEW_RANGE;
}


Uint32 timerCallback( Uint32 interval, void *param) {
	log3("tick\n");
	SDL_PushEvent( &timerPushEvent);
	return interval;
}

void level_startTimer() {
	levelTimer = SDL_AddTimer( timerDelay, timerCallback, 0);
}

void level_endTimer() {
	SDL_RemoveTimer( levelTimer);
}

void gameOver( int levelEndValue) {
	isPlayerPosSet = false;
	level_endTimer();

	SDL_Event event = {
		.type = SDL_USEREVENT,
	};
	event.user.code = CUSTOM_EVENT_ENDLEVEL;

	int *iPtr = (int*)malloc( sizeof(int));
	*iPtr = levelEndValue;

	event.user.data1 = iPtr;

	SDL_PushEvent( &event);
}

bool kick( struct Map *map, struct object *obj) {
	struct Vector *initPos = GET_PF_POS( map->pfBase[ obj->pos.i ][ obj->pos.j ]->neighbours[ obj->dir ] );

	if (initPos != NULL) {
		struct object *objHit = map->objs[initPos->i][initPos->j];

		if (objHit && objHit->isMovable) {
			struct Vector curPos;
			struct Vector prevPos;
			struct Vector *dirVector = &dirVectors[obj->dir];

			vectorClone(&curPos, initPos);

			do {
				vectorClone(&prevPos, &curPos);
				vectorAdd(&curPos, &curPos, dirVector);
			}
			while (map->tiles[curPos.i][curPos.j] == terrain_gnd &&
				map->objs[curPos.i][curPos.j] == NULL);

			if (vectorEquals(&prevPos, &curPos) != true) {
				map->objs[objHit->pos.i][objHit->pos.j] = NULL;
				map->objs[prevPos.i][prevPos.j] = objHit;

				vectorClone( &(objHit->pos), &prevPos );

				return true;
				PLAY_AUDIO_FX(AUDIO_HIT, 1);
			}
		}
	}

	PLAY_AUDIO_FX( AUDIO_NO_INTERACT, 0);
	return false;
}

/* Uses the object in-front of player */
bool interact( struct Map *map, struct object *obj) {
	struct BasePfNode *nextNode = map->pfBase[ obj->pos.i][ obj->pos.j]->neighbours[ obj->dir];
	struct Vector *pos = GET_PF_POS( nextNode);
	
	if( pos != NULL && map->objs[ pos->i][ pos->j]) {
		objectInteract( player, map->objs[ pos->i][ pos->j], lua);
		PLAY_AUDIO_FX( AUDIO_INTERACT, PLAYER_DISTANCE( pos));
		return true;
	}
	else {
		PLAY_AUDIO_FX( AUDIO_NO_INTERACT, PLAYER_DISTANCE( &obj->pos));
		return false;
	}
}

/* picks up the object in front of object*/
bool pickUp( struct Map *map, struct object *obj) {
	struct BasePfNode *nextNode = map->pfBase[ obj->pos.i][ obj->pos.j]->neighbours[ obj->dir];
	struct Vector *pos = GET_PF_POS( nextNode);
	
	struct object *pickedObj = map->objs[ pos->i][ pos->j];
	if( pos != NULL && pickedObj != NULL && pickedObj->isPickable && inventory_add( pickedObj, lua) ) {
		ARRAY_REMOVE( myMap->objList, pickedObj, myMap->objListCount);
		myMap->objs[ pos->i][ pos->j] = NULL;
		PLAY_AUDIO_FX( AUDIO_PICK, PLAYER_DISTANCE( pos));
		return true;
    }
	else {
		PLAY_AUDIO_FX( AUDIO_NO_PICK, PLAYER_DISTANCE( &obj->pos));
		return false;
	}
}

bool dropItem( struct Map *map, struct object *droppingObj, int inventoryIndex) {
	struct BasePfNode *nextNode = map->pfBase[ droppingObj->pos.i][ droppingObj->pos.j]->neighbours[ droppingObj->dir];
	struct Vector *pos = GET_PF_POS( nextNode);
	
	if( pos != NULL && map->objs[ pos->i][ pos->j] == NULL) {
		struct object *droppedObj = inventory_remove( inventoryIndex, lua);
		if( droppedObj != NULL) {
			droppedObj->dir = DIR_REVERSE( player->dir);
			vectorClone( &droppedObj->pos, pos);
			addObject( droppedObj, map, pos->i, pos->j);
			PLAY_AUDIO_FX( AUDIO_DROP, PLAYER_DISTANCE( pos));
			return true;
		}
	}
	PLAY_AUDIO_FX( AUDIO_NO_DROP, PLAYER_DISTANCE( &droppingObj->pos));
	return false;
}

/* Moves forward only if that spot is empty */
bool moveBackward( struct Map *map, struct object* obj) {
	struct BasePfNode *nextPfNode = map->pfBase[ obj->pos.i][ obj->pos.j]->neighbours[ DIR_REVERSE( obj->dir)];
	struct Vector *pos = GET_PF_POS(nextPfNode);
	if( pos != NULL && map->objs[ pos->i ][ pos->j ] == NULL) {
		map->objs[ obj->pos.i ][ obj->pos.j ] = 0;
		map->objs[ pos->i ][ pos->j ] = obj;
		vectorClone( &obj->pos, pos);
		PLAY_AUDIO_FX( AUDIO_MOVE, PLAYER_DISTANCE( pos));
		return true;
	}
	else {
		PLAY_AUDIO_FX( AUDIO_NO_MOVE, PLAYER_DISTANCE( &obj->pos));
		return false;
	}
}

/* Move forward. Hit the object if can't move forward */
bool moveForward( struct Map *map, struct object* obj) {
	struct BasePfNode *nextPfNode = map->pfBase[ obj->pos.i ][ obj->pos.j ]->neighbours[ obj->dir ];
	struct Vector *pos = GET_PF_POS( nextPfNode);

	if( pos != NULL ) {
		struct object *objAtPos = map->objs[ pos->i ][ pos->j ];
		if( ! objAtPos ) {
			map->objs[ obj->pos.i ][ obj->pos.j ] = 0;
			map->objs[ pos->i ][ pos->j ] = obj;
			vectorClone( &obj->pos, pos );
			PLAY_AUDIO_FX( AUDIO_MOVE, PLAYER_DISTANCE( pos));
			return true;
		}
		else {
			if (objectHit( obj, objAtPos)) {
				if (objAtPos == player && objAtPos->health == 0 ) {
					gameOver( 0);
				}
				PLAY_AUDIO_FX( AUDIO_HIT, PLAYER_DISTANCE( &obj->pos));
			}
			else {
				PLAY_AUDIO_FX( AUDIO_NO_MOVE, PLAYER_DISTANCE( &obj->pos));
			}
			return false;
		}
	}
	else {
		PLAY_AUDIO_FX( AUDIO_NO_MOVE, PLAYER_DISTANCE( &obj->pos));
		return false;
	}
}

bool strafeRight( struct Map *map, struct object *obj) {
	struct BasePfNode *nextNode = map->pfBase[ obj->pos.i][ obj->pos.j]->neighbours[ DIR_ROTATE_RIGHT( obj->dir) ];
	
	if( nextNode != NULL) {
		struct Vector *pos = &nextNode->pos;

		if( map->objs[ pos->i][ pos->j] == NULL ) {
			map->objs[ obj->pos.i][ obj->pos.j] = NULL;
			map->objs[ pos->i][ pos->j] = obj;
			vectorClone( &obj->pos, &nextNode->pos);
			
			PLAY_AUDIO_FX( AUDIO_MOVE, PLAYER_DISTANCE( pos));
			return true;
		}
	}

	PLAY_AUDIO_FX( AUDIO_NO_MOVE, PLAYER_DISTANCE( &obj->pos));
	return false;
}

bool strafeLeft( struct Map *map, struct object *obj) {
	struct BasePfNode *nextNode = map->pfBase[ obj->pos.i][ obj->pos.j]->neighbours[ DIR_ROTATE_LEFT( obj->dir) ];
	
	if( nextNode != NULL) {
		struct Vector *pos = &nextNode->pos;

		if( map->objs[ pos->i][ pos->j] == NULL ) {
			map->objs[ obj->pos.i][ obj->pos.j] = NULL;
			map->objs[ pos->i][ pos->j] = obj;
			vectorClone( &obj->pos, pos);
			
			PLAY_AUDIO_FX( AUDIO_MOVE, PLAYER_DISTANCE( pos));
			return true;
		}
	}

	PLAY_AUDIO_FX( AUDIO_NO_MOVE, PLAYER_DISTANCE( &obj->pos));
	return false;
}

bool pushForward( struct Map *map, struct object* pusher) {
	struct BasePfNode *nextBase = map->pfBase[ pusher->pos.i ][ pusher->pos.j ]->neighbours[ pusher->dir ];

	if( nextBase != NULL ) {
		struct BasePfNode *nextNextBase = nextBase->neighbours[ pusher->dir];
		struct object *pushedObj = map->objs[ nextBase->pos.i][ nextBase->pos.j];
		if( nextNextBase != NULL && pushedObj != NULL && pushedObj->isMovable ) {
			struct object *nextNextObj = map->objs[ nextNextBase->pos.i][ nextNextBase->pos.j];
			if( nextNextObj == NULL) {
				map->objs[ nextNextBase->pos.i][ nextNextBase->pos.j] = pushedObj;
				map->objs[ pushedObj->pos.i][ pushedObj->pos.j] = pusher;
				map->objs[ pusher->pos.i][ pusher->pos.j] = NULL;
				vectorClone( &pusher->pos, &pushedObj->pos);
				vectorClone( &pushedObj->pos, &nextNextBase->pos);
				PLAY_AUDIO_FX( AUDIO_PUSH, PLAYER_DISTANCE( &pushedObj->pos));
				return true;
			}
		}
	}
	PLAY_AUDIO_FX( AUDIO_NO_PUSH, PLAYER_DISTANCE( &pusher->pos));
	return false;
}

bool turnLeft( struct Map *map, struct object *obj) {
	obj->dir = DIR_ROTATE_LEFT(obj->dir);
	PLAY_AUDIO_FX( AUDIO_MOVE, PLAYER_DISTANCE( &obj->pos));
	return true;
}

bool turnRight( struct Map *map, struct object *obj) {
	obj->dir = DIR_ROTATE_RIGHT(obj->dir);
	PLAY_AUDIO_FX( AUDIO_MOVE, PLAYER_DISTANCE( &obj->pos));
	return true;
}

bool eat( struct Map *map, struct object *obj) {
	struct Vector newPos;
	vectorAdd(  &newPos, &obj->pos, &dirVectors[ obj->dir ]);
	struct object *otherObj = map->objs[newPos.i][newPos.j];
	if( otherObj != 0 && otherObj->health == 0) {
		objectSwallow( obj, otherObj, lua);
		map->objs[ newPos.i ][ newPos.j ] = NULL;
		PLAY_AUDIO_FX( AUDIO_EAT, PLAYER_DISTANCE( &newPos));
		return true;
	}

	PLAY_AUDIO_FX( AUDIO_NO_EAT, PLAYER_DISTANCE( &newPos));
	return false;
}

void movePlayer( moveFun *moveFunction ) {
	if( ! playerMoved) {
		playerMoved = true;
		moveFunction( myMap, player);
		CALL_FOV_FCN();
	}
}

void movePlayerPos( enum direction dir, bool shift) {
	if( ! playerMoved) {
		playerMoved = true;

		if (moveMode == MOVE_MODE_SIMPLE) {
			dir = (dir - player->dir)%4;
		}

		moveFun *moveFunction;

		switch (dir) {
			case 0:
				moveFunction = shift ? pushForward : moveForward;
				break;
			case 1:
				moveFunction = strafeRight;
				break;
			case 2:
				moveFunction = moveBackward;
				break;
			case 3:
				moveFunction = strafeLeft;
				break;
		};

		moveFunction( myMap, player);
		CALL_FOV_FCN();
	}
}


void handleKey( SDL_KeyboardEvent *e) {
	switch (e->keysym.scancode) {
		case 42:
			quit("pressed 'backspace'. Quitting");
			break;
		case 26:
			movePlayerPos( dir_up, e->keysym.mod & KMOD_LSHIFT);
			break;
		case 22:
			movePlayerPos( dir_down, e->keysym.mod & KMOD_LSHIFT);
			break;
		case 4:
			movePlayerPos( dir_left, e->keysym.mod & KMOD_LSHIFT);
			break;
		case 7:
			movePlayerPos( dir_right, e->keysym.mod & KMOD_LSHIFT);
			break;
		case 30:
			dropItem( myMap, player, 0);
			break;
		case 31:
			dropItem( myMap, player, 1);
			break;
		case 32:
			dropItem( myMap, player, 2);
			break;
		case 33:
			dropItem( myMap, player, 3);
			break;
		case 34:
			dropItem( myMap, player, 4);
			break;
		case 35:
			dropItem( myMap, player, 5);
			break;
		default:
			if (keyLookup[e->keysym.scancode]) {
				//movePlayer(keyLookup[e->keysym.scancode][e->keysym.mod & KMOD_LSHIFT]);
				movePlayer(keyLookup[e->keysym.scancode]);
			}
			break;
	};
}

void draw() {
	log3("draw here\n");
	SDL_RenderClear( renderer);

	int i, j; //index of tile in map
	int vi, vj; //where to draw tile in view
	
	struct ViewObject *vo, vo2;
	struct object *obj;

	struct TextureSheet *ts;
	SDL_Rect destRect = (SDL_Rect){.w = TILELEN};

	for (i=0, vi=vi_padding; i < PLAYER_FOV_TILES_LIM; i++, vi++ ) {
		for (j=0, vj=vj_padding; j < PLAYER_FOV_TILES_LIM; j++, vj++) {

			vo = &playerVisibleTiles[i][j];

			if (vo->terrain != terrain_dark) {

				/* draw terrain */
				drawTexture( renderer, textures->trn[ vo->terrain ], vi*TILELEN, vj*TILELEN, TILELEN, TILELEN );

				/* draw object */
				if (vo->obj) {
					obj = vo->obj;
					ts = textures->obj[obj->textureId];

					destRect.x = vi * TILELEN;
					destRect.y = (vj - ts->tallness) * TILELEN;
					destRect.h = (ts->tallness + 1)* TILELEN;

					int textureAlphaMod = 255;

					if (obj != highlitObject) {
						int dy;
						for (dy = 1; dy <= ts->tallness; dy ++) {
							vo2 = playerVisibleTiles[i][j - dy];

							if ((vo2.obj == NULL && makeTransparentForGround) || 
							    ( playerVisibleTiles[i][j - dy].obj &&
							      textures->obj[ playerVisibleTiles[i][j - dy].obj->textureId ]->tallness <= (ts->tallness - dy)
							    )) {

								textureAlphaMod /= 2;
							}
						}
					}

					SDL_SetTextureAlphaMod(ts->textures[obj->visualState][obj->dir], textureAlphaMod);

					SDL_RenderCopy(renderer, 
					               vo->isFullySeen
					                ? ts->textures[ obj->visualState ][ obj->dir ]
					                : textures->unidentifiedObj, 
					               &vo->srcRect, &destRect);

					if (obj->ai)
						AI_SEEN(obj->ai);
				}
			}
		}
	}
		
	int xStart = (windowW - CONSOLE_WIDTH) /2;
	int xEnd = (windowW + CONSOLE_WIDTH) /2;

	drawTexture( renderer, textConsole_texture, xStart, windowH - CONSOLE_HEIGHT, CONSOLE_WIDTH, CONSOLE_HEIGHT);

	/* the inventory */
	drawTexture( renderer, textures->inventory, xEnd - 7*TILELEN, 0, 7*TILELEN, TILELEN);
	for( i=0; i<INVENTORY_SIZE; i++) {
		if( inventory[i] != NULL)
			drawTexture( renderer, textures->obj[ inventory[i]->textureId]->textures[ 1][0], xEnd - (6-i)*TILELEN, 0, TILELEN, TILELEN);
	}

	/* the health bar */
	for( i=0; i<player->health; i++)
		drawTexture( renderer, textures->fullHeart, xStart + i*TILELEN, 0, TILELEN, TILELEN);
	for( ; i<player->maxHealth; i++)
		drawTexture( renderer, textures->emptyHeart, xStart + i*TILELEN, 0, TILELEN, TILELEN);

	SDL_RenderPresent( renderer);
}

void update() {
	log3("update\n");

	
	unsigned int i;

	struct object **newObjList = (struct object**)calloc( myMap->objListSize, sizeof( struct object*)); //TODO instead of reallocating this array, keep it and swap between it and the actual list
	int newCount = 0;

	struct object *obj;
	for( i=0; i<myMap->objListCount; i++) {
		obj = myMap->objList[i];
		if( obj->isDeleted != true) {
			newObjList[ newCount] = obj;
			newCount++;
		}
		else {
			free( obj);
		}

		if( obj->ai && obj->ai->enabled) {
			
			if( obj->timerCounter == 0) {
				AI_UPDATE( myMap, obj );
			}
			obj->timerCounter --;
		}
	}

	if( newCount != myMap->objListCount) {
		free( myMap->objList);
		myMap->objList = newObjList;
		myMap->objListCount = newCount;
	}
	else {
		free( newObjList);
	}

	clearFovObjects( playerVisibleTiles, VIEW_RANGE );
	getFovObjects( myMap, &player->pos, playerVisibleTiles, VIEW_RANGE, textures->obj);

	playerMoved = false;
}

void recalculateViewSize() {
	int w, h;
	SDL_GetWindowSize( window, &w, &h);
	resizeView(0, 0, w, h);
	setViewPaddings();
}

int run() {
	if( isPlayerPosSet != true) {
		quit( "player position should have been set from the level script.");
	}
	CALL_FOV_FCN();
	
	recalculateViewSize();

	SDL_Event e;
	while( true) {
		SDL_WaitEvent( &e);
		switch (e.type) {
			case SDL_WINDOWEVENT:
				switch( e.window.event) {
					case SDL_WINDOWEVENT_EXPOSED:
					    //log1("Window %d exposed\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_HIDDEN:
					    log1("Window %d hidden\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_SHOWN:
					    log1("Window %d shown\n", e.window.windowID);
						break;
					case SDL_WINDOWEVENT_RESIZED:
					    log1("Window %d resized to %dx%d\n", e.window.windowID, e.window.data1, e.window.data2);
						resizeView( 0, 0, e.window.data1, e.window.data2);
						setViewPaddings();
					    break;
					case SDL_WINDOWEVENT_MINIMIZED:
					    log1("Window %d minimized\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_MAXIMIZED:
					    log1("Window %d maximized\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_RESTORED:
					    log1("Window %d restored\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_CLOSE:
					    log0("Window %d closed\n", e.window.windowID);
					    break;
					default:
						//unhandled window event
						continue;
				};
				break;
			case SDL_USEREVENT:
                if( e.user.code == CUSTOM_EVENT_UPDATE) 
                    update();
                else {
                    int *iPtr = (int*)(e.user.data1);
					int i = *iPtr;
					free( iPtr);
					return i;
				}
				break;
			case SDL_QUIT:
				quit("Quitting");
				break;
			case SDL_KEYDOWN:
				handleKey( (SDL_KeyboardEvent*)&e);
				break;
			case SDL_MOUSEMOTION: 
				if( playerMoved != true) {
					mouseTile.i = e.motion.x / TILELEN - vi_padding;
					mouseTile.j = e.motion.y / TILELEN - vj_padding;

					if (mouseTile.i >= 0 &&
					    mouseTile.j >= 0 &&
					    mouseTile.i < VIEW_BOX_LENGTH &&
					    mouseTile.j < VIEW_BOX_LENGTH) {

						highlitObject = playerVisibleTiles[ mouseTile.i ][ mouseTile.j ].obj;
					}

					enum direction newDir = vector_dirTan( mouseTile.j - VIEW_RANGE, mouseTile.i - VIEW_RANGE);
					if( player->dir == newDir)
						continue;
					player->dir = newDir;
					playerMoved = true;
					CALL_FOV_FCN();
					break;
				}
			case SDL_MOUSEBUTTONDOWN:
				if( e.button.state == SDL_PRESSED) {
					if( e.button.button == SDL_BUTTON_LEFT) {
						movePlayer( interact);
					}
					break;
				}
			case SDL_KEYUP:
			case SDL_MOUSEBUTTONUP:
			default:
				continue;
		};
		draw();
	}
	
	return 0 /*TODO rename to DEFAULT_ENDLEVEL_VALUE*/;
}


void setDefaults() {
	log1("setting defaults\n");

	isPlayerPosSet = false;
	currentFov = fov_raycast;
	setViewRange( VIEW_RANGE_DEFAULT );

	timerPushEvent.user.code = CUSTOM_EVENT_UPDATE;

	playerVisibleTiles = (struct ViewObject**) calloc( PLAYER_FOV_TILES_LIM, sizeof(struct ViewObject*));
	int i, j;
	for (i=0; i< PLAYER_FOV_TILES_LIM; i++) {
		playerVisibleTiles[i] = (struct ViewObject*) calloc( PLAYER_FOV_TILES_LIM, sizeof(struct ViewObject));

		for (j = 0; j < PLAYER_FOV_TILES_LIM; j ++) {
			struct ViewObject *vo = &playerVisibleTiles[i][j];

			vo->obj = NULL;
			vo->pos.i = i;
			vo->pos.j = j;
			vo->terrain = terrain_dark;
			vo->srcRect.w = TILELEN;
			vo->isFullySeen = false;
		}
	}
	
	init_fovBase( VIEW_RANGE);

	inventory_reset( false);

	for (i=0; i<KEY_LOOKUP_SIZE; i++) {
		keyLookup[i] = NULL;
	}
	keyLookup[ 6 /*C*/] = kick;
	keyLookup[ 9 /*F*/] = pickUp;
	keyLookup[10 /*G*/] = eat;
}

int main( int argc, char *args[]) {
	
	if( argc != 2) {
		fprintf( stderr, "Usage: %s map-file (without .yz.* extensions)", args[0]);
		return 1;
	}

	dirPath = getDirPath( args[1]);

	char *spriteSpecsPath = calculateSpriteSpecsFilePath( args[1] );
	spriteSpecs = readSpriteSpecsFile( spriteSpecsPath );
	if (spriteSpecs == NULL) {
		fprintf(stderr, "Texture-Specification-File at '%s' couldn't be loaded\n", spriteSpecsPath);
		return 1;
	}
	free( spriteSpecsPath);

	if(validateSpriteSpecs(spriteSpecs) != true) {
		fprintf(stderr, "Ending program because of errors in texture-scheduler specification\n");
		return 1;
	}

	lua = initLua();
	setDefaults();
	
	init( 0, 0, 1280, 960);
	textures = loadOrdinaryTextures( renderer);
	loadObjectTextures( renderer, textures, spriteSpecs);
	audio_init();
	cutscene_init();
	textConsole_init( renderer);

	if (luaL_loadfile(lua, args[1]) || lua_pcall( lua, 0, 0, 0)) {
		fprintf(stderr, "Error loading script '%s'\n%s\n", args[1], lua_tostring(lua, -1));
		return 0;
	}
	
	log0("Program over\nDeallocating because I'm just OCD like that\n");
	audio_free();
	freeTextures( textures);
	textConsole_destroy( renderer);
	SDL_RenderClear( renderer);
	SDL_DestroyRenderer( renderer);
	SDL_DestroyWindow( window);
	free_fovBase( fovBase);
	if(myMap)
		freeMap( myMap);
	free( dirPath);
	return 0;
}
