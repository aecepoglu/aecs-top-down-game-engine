#include "dsl.h"

#include "text.h"
#include "textConsole.h"
#include "cutscene.h"
#include "util/log.h"
#include "core/engine.h"
#include "core/audio.h"
#include "ai/aiTable.h"
#include "inventory.h"
#include "vector.h"
#include "game.h"


#define MAX_PATH_LEN 256

#define FOREACH_OBJ_WITH_ID( objId, itI, itObj, closure) for( itI=0; itI<myMap->objListCount ;itI++) {\
	if( myMap->objList[itI]->id == objId) {\
		itObj = myMap->objList[itI];\
		closure\
	}\
}

int loadLevel( const char* relMapPath, const char* relScriptPath, int levelOption, lua_State *L) {
    char fullPath[MAX_PATH_LEN];
	
    sprintf( fullPath, "%s/%s", dirPath, relMapPath);

	if( myMap != NULL && strcmp( fullPath, myMap->filePath) != 0) {
		freeMap( myMap);
		myMap = NULL;
	}

	level_startTimer();

	myMap = readMapFile( fullPath);

	player = createObject( go_player, 0, 0, 0);

    sprintf( fullPath, "%s/%s", dirPath, relScriptPath);
    if (luaL_loadfile(L, fullPath) || lua_pcall( L, 0, 0, 0)) {
        fprintf(stderr, "Error loading script '%s'\n%s\n", fullPath, lua_tostring(L, -1));
        exit(1);
	}

	lua_getglobal( L, "init");
	if( lua_isnil( L, -1) != true) {
		lua_pushinteger( L, levelOption);
		lua_pcall( L, 1, 0, 0 );
	}

    return run();
}

/* this is available for debug reasons only */
int luaStackDump(lua_State* l)
{
    int i;
    int top = lua_gettop(l);
 
    printf("----total in stack %d----\n",top);
 
    for (i = 1; i <= top; i++)
    {  /* repeat for each level */
        printf("%d. ", i);
        int t = lua_type(l, i);
        switch (t) {
            case LUA_TSTRING:  /* strings */
                printf("string: '%s'", lua_tostring(l, i));
                break;
            case LUA_TBOOLEAN:  /* booleans */
                printf("boolean %s",lua_toboolean(l, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:  /* numbers */
                printf("number: %g", lua_tonumber(l, i));
                break;
            default:  /* other values */
                printf("%s", lua_typename(l, t));
                break;
        }
        printf("\n");
    }
    printf("-----------------------\n");  /* end the listing */
    return 0;
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

int dsl_startLevel( lua_State *l) {
	luaL_checkstring( l, 1);
	luaL_checkstring( l, 2);
	luaL_checkinteger( l, 3);
	
	const char *mapPath = lua_tostring( l, 1);
	const char *scriptPath = lua_tostring( l, 2);
	int mapStartOption = lua_tointeger( l, 3);
	
	lua_pushinteger( l, loadLevel( mapPath, scriptPath, mapStartOption, l) );
	
	return 1;
}

static int dsl_endLevel( lua_State *l) {
	luaL_checkinteger( l, 1);

	int levelEndValue = lua_tointeger( l, 1);
	gameOver( levelEndValue);
	return 0;
}

int dsl_setStartGate( lua_State *l) {
	luaL_checkinteger( l, 1);

	int gateId = lua_tointeger( l, 1);
	
	struct object *o;
	int i;
	FOREACH_OBJ_WITH_ID( gateId, i, o, {
		struct BasePfNode *startBase = myMap->pfBase[ o->pos.i][ o->pos.j]->neighbours[ o->dir];
		if( startBase == NULL) {
			fprintf( stderr, "Cannot start from gate %d. It's looking at a non-ground tile", gateId);
			exit( 1);
		}
		else if ( myMap->objs[ startBase->pos.i][ startBase->pos.j] != NULL) {
			fprintf( stderr, "Cannot start from gate %d. The position is not empty", gateId);
			exit( 1);
		}
		vectorClone( &player->pos, &startBase->pos);
		player->dir = o->dir;
		addObject( player, myMap, player->pos.i, player->pos.j);
		isPlayerPosSet = true;
		return 0;
		}
	)
	fprintf( stderr, "Cannot start from gate %d. No object with such id is found", gateId);
	exit( 1);
	return 0;
}

int dsl_useObject( lua_State *l) {
	luaL_checkinteger( l, 1);
	int usedObjId = lua_tointeger( l, 1);

	int i;
	struct object *o;
	FOREACH_OBJ_WITH_ID( usedObjId, i, o, {
		if( o->ai) {
			AI_USE( myMap, o);
		}
	})
	return 0;
}

int dsl_writeConsole( lua_State *l) {
	luaL_checkstring( l, 1);
	const char *str = lua_tostring( l, 1);
	
	textConsole_add( str);
	textConsole_write( renderer, textures->font, textConsole_texture);
	PLAY_AUDIO_FX( AUDIO_CONSOLE, 0);
	return 0;
}

int dsl_clearConsole( lua_State *l) {
	int i;
	for( i=0; i<CONSOLE_NUM_ROWS; i++) {
		textConsole_add("");
	}
	textConsole_write( renderer, textures->font, textConsole_texture);
	PLAY_AUDIO_FX( AUDIO_CONSOLE, 0);
	return 0;
}

int dsl_listInventory( lua_State *l) {
	lua_createtable( l, INVENTORY_SIZE, 0);
	
	int i;
	for( i=0; i<INVENTORY_SIZE; i++) {
		lua_pushinteger( l, inventory[i] != NULL ? inventory[i]->id : 0);
		lua_rawseti( l, 1, i);
	}

	return 1;
}

int dsl_onInventoryAdd( lua_State *l) {
	luaL_checktype( l, 1, LUA_TFUNCTION);
	
	inventoryCallbacks.onAdd = luaL_ref( l, LUA_REGISTRYINDEX);

	return 0;
}

int dsl_onInventoryRemove( lua_State *l) {
	luaL_checktype( l, 1, LUA_TFUNCTION);

	inventoryCallbacks.onRemove = luaL_ref( l, LUA_REGISTRYINDEX);

	return 0;
}

int dsl_setObjTextures( lua_State *l) {
	luaL_checktype( l, 1, LUA_TTABLE);

	lua_pushnil(l);
	while( lua_next( l, 1) ) {
		enum objType type = lua_tointeger( l, -2);
		const char *texturePath = lua_tostring( l, -1);
		if( type >= 0 && type < go_NUM_ITEMS) {
			struct TextureSheet* newSheet = loadObjTextures( renderer, texturePath);
			if( newSheet->numStates == textures->obj[ type]->numStates) {
				texture_freeTextureSheet( textures->obj[ type]);
				textures->obj[ type] = newSheet;
			}
			else {
				fprintf( stderr, "Cannot set texture for object with type :%d because the texture-sheet at given path: '%s' has %d states only. It should be %d\n", type, texturePath, newSheet->numStates, textures->obj[ type]->numStates);
				texture_freeTextureSheet( newSheet);
			}
		}
		lua_pop( l, 1);
	}
	return 0;
}

int dsl_setTileTextures( lua_State *l) {
	luaL_checktype( l, 1, LUA_TTABLE);

	lua_pushnil(l);
	while( lua_next( l, 1) ) {
		enum terrainType type = lua_tointeger( l, -2);
		const char *texturePath = lua_tostring( l, -1);
		if( type >= 0 && type < terrain_NUM_ITEMS) {
			SDL_DestroyTexture( textures->trn[ type]);
			textures->trn[ type] = loadTexture( renderer, texturePath);
		}
		lua_pop( l, 1);
	}
	return 0;
}

int dsl_changeAIStatus( lua_State *l) {
	luaL_checkinteger( l, 1);
	luaL_checktype( l, 2, LUA_TBOOLEAN);

	int objId = lua_tointeger( l, 1);
	bool newEnabledValue = lua_toboolean( l, 2);

	int i;
	struct object *o;
	FOREACH_OBJ_WITH_ID( objId, i, o, {
		if( o->ai != NULL && aiTable[ o->ai->type].updateFun != NULL) {
			o->ai->enabled = newEnabledValue;
		}
	});

	return 0;
}

int dsl_cutsceneWait( lua_State *l) {
	luaL_checkinteger( l, 1);

	int miliseconds = lua_tointeger( l, 1);
	cutscene_wait( renderer, miliseconds, false);

	return 0;
}


int dsl_cutsceneClear( lua_State *l) {
	luaL_checkinteger( l, 1);
	luaL_checkinteger( l, 2);
	luaL_checkinteger( l, 3);

	cutscene.bgColor.r = lua_tointeger( l, 1);
	cutscene.bgColor.g = lua_tointeger( l, 2);
	cutscene.bgColor.b = lua_tointeger( l, 3);

	cutscene_clear();

	return 0;
}

int dsl_cutsceneWrite( lua_State *l) {
	luaL_checkinteger( l, 1);
	luaL_checkinteger( l, 2);
	luaL_checkstring( l, 3);

	struct CutsceneElement *elem = (struct CutsceneElement*)malloc( sizeof( struct CutsceneElement));
	elem->rect.x = lua_tointeger( l, 1);
	elem->rect.y = lua_tointeger( l, 2);
	
	const char *text = lua_tostring( l, 3);

	elem->texture = getTextTexture( renderer, textures->font, text, 18, 24, (int[4]){0,0,0,0}, 255, 255, 255, &elem->rect.w, &elem->rect.h);

	cutscene_add( elem);

	return 0;
}

int dsl_cutsceneImg( lua_State *l) {
	luaL_checkinteger( l, 1);
	luaL_checkinteger( l, 2);
	luaL_checkinteger( l, 3);
	luaL_checkinteger( l, 4);
	luaL_checkstring( l, 5);

	struct CutsceneElement *elem = (struct CutsceneElement*)malloc( sizeof( struct CutsceneElement));

	elem->rect.x = lua_tointeger( l, 1);
	elem->rect.y = lua_tointeger( l, 2);
	elem->rect.w = lua_tointeger( l, 3);
	elem->rect.h = lua_tointeger( l, 4);
	elem->texture = loadTexture( renderer, lua_tostring( l, 5));

	cutscene_add( elem);
	
	return 0;
}

int dsl_cutsceneRender( lua_State *l) {
	cutscene_draw( renderer);
	return 0;
}

int dsl_cutsceneReadKey( lua_State *l) {
	lua_pushinteger( l, cutscene_wait( renderer, 0, true) );
	return 1;
}

int dsl_getObjAtPos( lua_State *l) {
	luaL_checkinteger( l, 1);
	luaL_checkinteger( l, 2);

	int x = lua_tointeger( l, 1);
	int y = lua_tointeger( l, 2);

	if( x >= 0 && x < myMap->width && y >= 0 && y < myMap->height ) {
		struct object *obj = NULL;
		obj = myMap->objs[ x][ y];
		lua_pushinteger( l, obj != NULL ? obj->id : -1 );
	}
	return 1;
}

int dsl_setFov( lua_State *l) {
	luaL_checkinteger( l, 1);

	int fovOption = lua_tointeger( l, 1);

	switch (fovOption) {
		case 0:
			currentFov = fov_line;
			break;
		case 1:
			currentFov = fov_raycast;
			break;
		case 2:
			currentFov = fov_diamond;
			break;
	};

	return 0;
}

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
		{"cutClear", dsl_cutsceneClear},
		{"cutWrite", dsl_cutsceneWrite},
		{"cutImg", dsl_cutsceneImg},
		{"cutRender", dsl_cutsceneRender},
		{"cutWait", dsl_cutsceneWait},
		{"cutReadKey", dsl_cutsceneReadKey},
		{"objAtPos", dsl_getObjAtPos},
		{"setFov", dsl_setFov},
		{"printStack", luaStackDump},
		{NULL, NULL}
	}, 0);
	
	lua_pushstring( L, "DIR_PATH");
	lua_pushstring( L, dirPath);
	lua_settable( L, -3);
	
	lua_setglobal( L, "lib");

	return L;
}
