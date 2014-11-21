#include "engine.h"
#include "aiTable.h"
#include "fov/fov.h"
#include "definitions.h"
#include "textConsole.h"
#include "inventory.h"
#include "array.h"
#include "dsl.h"

#define FOREACH_OBJ_WITH_ID( objId, itI, itObj, closure) for( itI=0; itI<myMap->objListCount ;itI++) {\
	if( myMap->objList[itI]->id == objId) {\
		itObj = myMap->objList[itI];\
		closure\
	}\
}

#define CUSTOM_EVENT_UPDATE 	0
#define CUSTOM_EVENT_ENDLEVEL	1

SDL_Event timerPushEvent = {
	.type= SDL_USEREVENT,
};
const Uint32 timerDelay = 100 /*miliseconds*/;

struct object *player;

/* The player is allowed to play only once per tick.
	This variable shows whether the player has moved or not */
bool playerMoved = false;
/* when a level is read, player position needs to be set from within the level script.
	until then, this variable stays false */
bool isPlayerPosSet = false;

//FIXME The program will crash if the window is too small (test if this is still happening). Render window un-usable and show a notification message about it.

#define PLAYER_FOV_TILES_LIM VIEW_BOX_LENGTH
enum terrainType **playerVisibleTiles;
struct ViewObject objsSeen[ VIEW_BOX_PERIMETER];
int objsSeenCount;

SDL_Texture *textConsole_texture;

#define CALL_FOV_FCN() fov_raycast( myMap, &player->pos, player->dir, VIEW_RANGE, playerVisibleTiles, objsSeen, &objsSeenCount)

struct {
	SDL_Rect container;
	SDL_Rect fov;
} guiMeasurements;


void gameOver( int levelEndValue) {
	isPlayerPosSet = false;

	SDL_Event event = {
		.type = SDL_USEREVENT,
	};
	event.user.code = 1;

	int *iPtr = (int*)malloc( sizeof(int));
	*iPtr = levelEndValue;

	event.user.data1 = iPtr;

	SDL_PushEvent( &event);
}

/* Uses the object in-front of player */
bool interact( struct Map *map, struct object *obj) {
	struct BasePfNode *nextNode = map->pfBase[ obj->pos.i][ obj->pos.j]->neighbours[ obj->dir];
	struct Vector *pos = GET_PF_POS( nextNode);
	
	if( pos != NULL && map->objs[ pos->i][ pos->j]) {
		objectInteract( player, map->objs[ pos->i][ pos->j], lua);
		return true;
	}
	else
		return false;
}

/* picks up the object in front of object*/
bool pickUp( struct Map *map, struct object *obj) {
	struct BasePfNode *nextNode = map->pfBase[ obj->pos.i][ obj->pos.j]->neighbours[ obj->dir];
	struct Vector *pos = GET_PF_POS( nextNode);
	
	struct object *pickedObj = map->objs[ pos->i][ pos->j];
	if( pos != NULL && pickedObj != NULL && inventory_add( pickedObj, lua) ) {
		
		ARRAY_REMOVE( myMap->objList, pickedObj, myMap->objListCount);
		myMap->objs[ pos->i][ pos->j] = NULL;
		return true;
    }
	else
		return false;
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
			return true;
		}
	}
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
		return true;
	}
	else
		return false;
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
			return true;
		}
		else {
			if (objectHit( obj, objAtPos) && objAtPos == player && objAtPos->health == 0 ) {
				gameOver( 0);
			}
			return false;
		}
	}
	else {
		return false;
	}
}

bool turnLeft( struct Map *map, struct object *obj) {
	obj->dir = DIR_ROTATE_LEFT(obj->dir);
	return true;
}

bool turnRight( struct Map *map, struct object *obj) {
	obj->dir = DIR_ROTATE_RIGHT(obj->dir);
	return true;
}

bool eat( struct Map *map, struct object *obj) {
	struct Vector newPos;
	vectorAdd(  &newPos, &obj->pos, &dirVectors[ obj->dir ]);
	struct object *otherObj = map->objs[newPos.i][newPos.j];
	if( otherObj != 0 && otherObj->health == 0) {
		objectSwallow( obj, otherObj);
		map->objs[ newPos.i ][ newPos.j ] = NULL;
	}
	return true;
}

void movePlayer( bool (moveFunction)(struct Map*, struct object*) ) {
	if( ! playerMoved) {
		playerMoved = true;
		moveFunction( myMap, player);
		CALL_FOV_FCN();
	}
}

void handleKey( SDL_KeyboardEvent *e) {
	switch (e->keysym.sym) {
		case SDLK_q:
            quit("pressed 'q'. Quitting");
			break;
		case SDLK_UP:
			movePlayer( moveForward);
			break;
		case SDLK_DOWN:
			movePlayer( moveBackward);
			break;
		case SDLK_LEFT:
			movePlayer( turnLeft);
			break;
		case SDLK_RIGHT:
			movePlayer( turnRight);
			break;
		case SDLK_e:
			eat( myMap, player);
			break;
        case SDLK_u:
            interact( myMap, player);
            break;
		case SDLK_p:
			pickUp( myMap, player);
			break;
		case SDLK_1:
			dropItem( myMap, player, 0);
			break;
		case SDLK_2:
			dropItem( myMap, player, 1);
			break;
		case SDLK_3:
			dropItem( myMap, player, 2);
			break;
		case SDLK_4:
			dropItem( myMap, player, 3);
			break;
		case SDLK_5:
			dropItem( myMap, player, 4);
			break;
		case SDLK_6:
			dropItem( myMap, player, 5);
			break;
		default:
			log0("Unhandled key\n");
			break;
	};
}

Uint32 timerCallback( Uint32 interval, void *param) {
	log3("tick\n");
	SDL_PushEvent( &timerPushEvent);
	return interval;
}

void draw() {
	log3("draw here\n");
	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255);
	SDL_RenderClear( renderer);

	int i, j; //index of tile in map
	int vi, vj; //where to draw tile in view
	
	#ifdef GOD_VISION //TODO try to optimize the calculations in here

	vi = MAX(player->pos.i-viewSize.i/2, 0);
	int vjStart = MAX(player->pos.j-viewSize.j/2, 0);

	for( i=vi - player->pos.i + viewSize.i/2; vi<=MIN(myMap->width-1,player->pos.i+viewSize.i/2); i++, vi++) {
		for( vj=vjStart, j=vj - player->pos.j + viewSize.j/2; vj<=MIN(myMap->height-1,player->pos.j+viewSize.j/2); j++, vj++) {
			drawTexture( renderer, textures->trn[ myMap->tiles[ vi][ vj]], i*TILELEN, j*TILELEN, TILELEN, TILELEN);
			if( myMap->objs[ vi][ vj]) {
				struct object *o = myMap->objs[ vi][ vj];
				drawTexture( renderer, textures->obj[ o->type]->textures[ o->visualState][ o->dir], i*TILELEN, j*TILELEN, TILELEN, TILELEN);
			}
		}
	}

	#endif

	int vi_padding = viewSize.i/2 - VIEW_RANGE; //TODO can be defined elsewhere. They only change value when VIEW_RANGE changes or view is resized
	int vj_padding = viewSize.j/2 - VIEW_RANGE; //TODO can be defined elsewhere. They only change value when VIEW_RANGE changes or view is resized

	for( i=0, vi=vi_padding; i<PLAYER_FOV_TILES_LIM; i++, vi++ )
		for( j=0, vj=vj_padding; j<PLAYER_FOV_TILES_LIM; j++, vj++) {
			if( playerVisibleTiles[ i][ j] != terrain_dark) {
				#ifdef GOD_VISION
				drawTexture( renderer, textures->highlitObjIndicator, vi*TILELEN, vj*TILELEN, TILELEN, TILELEN);
				#else
				drawTexture( renderer, textures->trn[ playerVisibleTiles[ i][ j]], vi*TILELEN, vj*TILELEN, TILELEN, TILELEN);
				#endif
			}
		}
				
	struct ViewObject *vo;
	for( i=0; i<objsSeenCount; i++) {
		vo = &objsSeen[ i];

		#ifndef GOD_VISION
		drawTexture( renderer, 
			vo->isFullySeen
				? textures->obj[ vo->obj->type ]->textures[ vo->obj->visualState][ vo->obj->dir]
				: textures->unidentifiedObj, 
			(vi_padding + vo->pos.i)*TILELEN, (vj_padding + vo->pos.j)*TILELEN, TILELEN, TILELEN
		);
		#endif
		
		if( vo->obj->ai)
			AI_SEEN( vo->obj->ai);
	}

	drawTexture( renderer, textConsole_texture, 20, 20, CONSOLE_WIDTH, CONSOLE_HEIGHT);
	for( i=0; i<INVENTORY_SIZE; i++) {
		drawTexture( renderer, 
			inventory[i] != NULL
				? textures->obj[ inventory[i]->type]->textures[ 1][0]
				: textures->highlitObjIndicator,
			0, 20 + CONSOLE_HEIGHT + i*TILELEN, TILELEN, TILELEN);
	}

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

	getFovObjects( myMap, &player->pos, playerVisibleTiles, VIEW_RANGE, objsSeen, &objsSeenCount);

	playerMoved = false;
}

int run() {
	if( isPlayerPosSet != true) {
		quit( "player position should have been set from the level script.");
	}
	CALL_FOV_FCN();

	SDL_AddTimer( timerDelay, timerCallback, 0);

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
						
						guiMeasurements.container.w = e.window.data1;
						guiMeasurements.container.h = e.window.data2;
						guiMeasurements.fov.x = (guiMeasurements.container.w - guiMeasurements.fov.w)/2;
						guiMeasurements.fov.y = (guiMeasurements.container.h - guiMeasurements.fov.h)/2;
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
				log1("key down\n");
				handleKey( (SDL_KeyboardEvent*)&e);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEMOTION:
			case SDL_KEYUP:
			case SDL_MOUSEBUTTONUP:
				/*don't do anything for these events*/
				continue;
			default:
				log1("unhandled event type: %d\n", e.type);
				continue;
		};
		draw();
	}
	
	return 0 /*TODO rename to DEFAULT_ENDLEVEL_VALUE*/;
}


void setDefaults() {
	log1("setting defaults\n");

	timerPushEvent.user.code = CUSTOM_EVENT_UPDATE;

	playerVisibleTiles = (enum terrainType**) calloc( PLAYER_FOV_TILES_LIM, sizeof( enum terrainType*));
	int i;
	for( i=0; i< PLAYER_FOV_TILES_LIM; i++)
		playerVisibleTiles[i] = (enum terrainType*) calloc( PLAYER_FOV_TILES_LIM, sizeof( enum terrainType));

	init_fovBase( VIEW_RANGE);

	guiMeasurements.fov = (SDL_Rect){.w = PLAYER_FOV_TILES_LIM * TILELEN, .h = PLAYER_FOV_TILES_LIM * TILELEN};
	guiMeasurements.container = (SDL_Rect){.x = 0, .y=0};

	inventory_reset( false);
}

static int dsl_onInteract( lua_State *l) {
	luaL_checkinteger( l, 1);
	luaL_checktype( l, 2, LUA_TFUNCTION);
	
	int objId = lua_tointeger( l, 1);
	
	log1("Setting trigger for obj with id %d.\n", objId);
	
	int i;
	struct object *o;
	int count=0;
	int ref = luaL_ref( l, LUA_REGISTRYINDEX);
	FOREACH_OBJ_WITH_ID( objId, i, o, {
		o->callbacks.onInteract = ref;
		count ++;
	})
	
	log1("Set interact-trigger for %d objects\n", count);

	return 0;
}

int loadLevel( const char* mapPath, const char* scriptPath, int levelOption, lua_State *L) {
	if( myMap != NULL && strcmp( mapPath, myMap->filePath) != 0) {
		freeMap( myMap);
		myMap = NULL;
	}
	
	myMap = readMapFile( mapPath);
	player = createObject( go_player, 0, 0, 0);

    if (luaL_loadfile(L, scriptPath) || lua_pcall( L, 0, 0, 0)) {
        fprintf(stderr, "Error loading script '%s'\n%s\n", scriptPath, lua_tostring(L, -1));
        exit(1);
	}

	lua_getglobal( L, "init");
	if( lua_isnil( L, -1) != true) {
		lua_pushinteger( L, levelOption);
		lua_pcall( L, 1, 0, 0 );
	}

    return run();
}

#include "dsl.c"

lua_State* initLua() {
	lua_State * L;
	
	L = luaL_newstate();
	luaL_openlibs( L ); 

	lua_newtable( L);
	luaL_setfuncs( L, (struct luaL_Reg[]) {
		{"use", dsl_useObject},
		{"onInteract", dsl_onInteract},
		{"startLevel", dsl_startLevel},
		{"endLevel", dsl_endLevel},
		{"setStartGate", dsl_setStartGate},
		{"write", dsl_writeConsole},
		{"clear", dsl_clearConsole},
		{"listInventory", dsl_listInventory},
		{"onInventoryAdd", dsl_onInventoryAdd},
		{"onInventoryRemove", dsl_onInventoryRemove},
		{"setObjTextures", dsl_setObjTextures},
		{"setTileTextures", dsl_setTileTextures},
		{"changeAIStatus", dsl_changeAIStatus},
		{"printStack", luaStackDump},
		{NULL, NULL}
	}, 0);
	lua_setglobal( L, "lib");

	return L;
}

int main( int argc, char *args[]) {
	
	if( argc != 2) {
		fprintf( stderr, "Usage: %s map-file (without .yz.* extensions)", args[0]);
        return 0;
	}
	lua = initLua();
	setDefaults();
	
	init();
	textures = loadAllTextures( renderer);
	textConsole_texture = textConsole_init( renderer);
    
    if (luaL_loadfile(lua, args[1]) || lua_pcall( lua, 0, 0, 0)) {
        fprintf(stderr, "Error loading script '%s'\n%s\n", args[1], lua_tostring(lua, -1));
        return 0;
	}
	
	log0("Program over\nDeallocating because I'm just OCD like that\n");
	freeTextures( textures);
	SDL_RenderClear( renderer);
	SDL_DestroyRenderer( renderer);
	SDL_DestroyWindow( window);
	free_fovBase( fovBase);
	freeMap( myMap);
	return 0;
}
