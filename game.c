#include "engine.h"
#include "aiTable.h"
#include "fov/fov.h"
#include "definitions.h"


SDL_Event timerPushEvent;
Uint32 timerDelay;

struct object *player;

/* The player is allowed to play only once per tick. This variable shows whether the player has moved or not */
bool playerMoved = false;

//FIXME The program will crash if the window is too small. Render window un-usable and show a notification message about it.

#define PLAYER_FOV_TILES_LIM VIEW_BOX_LENGTH
enum terrainType **playerVisibleTiles;
struct ViewObject objsSeen[ VIEW_BOX_PERIMETER];
int objsSeenCount;

#define CALL_FOV_FCN() fov_raycast( myMap, &player->pos, player->dir, VIEW_RANGE, playerVisibleTiles, objsSeen, &objsSeenCount)

void gameOver() {
	log0("game over...\n");
	running=false;
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
			if (objectHit( obj, objAtPos) ) {
				//if objAtPos died
				if( objAtPos->health == 0 ) {
					log0("object died\n");
					if( objAtPos == player ) {
						gameOver();
					}
					else if(objAtPos->ai){
						AI_DESTROY(objAtPos->ai);
						objAtPos->ai=NULL;
					}
				}
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
		/* mark it for 'awaiting deletion'
		It will be deleted in the next update cycle */
		otherObj->isDeleted=true;
		if( otherObj == player) {
			gameOver();
		}
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
			running = 0;
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
	SDL_RenderClear( renderer);

	drawTexture( renderer, textures->trn[TEXTURE_TRN_NONE], 0, 0, windowW, windowH);

	unsigned int i,j;
	for( i=0; i<PLAYER_FOV_TILES_LIM; i++ )
		for( j=0; j<PLAYER_FOV_TILES_LIM; j++)
			drawTexture( renderer, textures->trn[ playerVisibleTiles[ i][ j]], 
				i*TILELEN, j*TILELEN, TILELEN, TILELEN
			);
	
	struct ViewObject *vo;
	for( i=0; i<objsSeenCount; i++) {
		vo = &objsSeen[ i];
		if( vo->isFullySeen)
			drawTexture( renderer, textures->obj[ vo->obj->type ][ vo->obj->health > 0 && vo->obj->visualState][ vo->obj->dir], vo->pos.i*TILELEN, vo->pos.j*TILELEN, TILELEN, TILELEN);
		else
			drawTexture( renderer, textures->obj[ go_apple ][ 0][ 0], vo->pos.i*TILELEN, vo->pos.j*TILELEN, TILELEN, TILELEN);
	}

	SDL_RenderPresent( renderer);
}

void update() {
	log3("update\n");

	
	unsigned int i;
	struct object *obj;
	for( i=0; i<myMap->objListCount; i++) {
		obj = myMap->objList[i];
		//update object at [i]
		/* Some objects are marked for deletion
		Copy all non-deleted objects into a new-list
		and that list is the new object-list */
		if( obj->isDeleted ) {
			//TODO implement obj deletion. Just not showing the deleted objects now
			//TODO don't forget to remove the matching code from draw()
		}
		else if( obj->ai && myMap->objList[i]->ai->enabled ) {
			if( obj->timerCounter == 0) {
				AI_SEEN( obj->ai);
				AI_UPDATE( myMap, obj );
			}
			myMap->objList[i]->timerCounter --;
		}
	}

	getFovObjects( myMap, &player->pos, playerVisibleTiles, VIEW_RANGE, objsSeen, &objsSeenCount);

	playerMoved = false;
}

void run() {
	CALL_FOV_FCN();

	SDL_AddTimer( timerDelay, timerCallback, 0);

	SDL_Event e;
	while( running) {
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
						resizeView(e.window.data1, e.window.data2);
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
					    log1("Window %d closed\n", e.window.windowID);
					    break;
					default:
						//unhandled window event
						continue;
				};
				break;
			case SDL_USEREVENT:
				update();
				break;
			case SDL_QUIT:
				quit("Quitting");
				running =0;
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
}


void setDefaults() {
	log0("setting defaults\n");
	
	timerDelay = 100;
	

	SDL_UserEvent userEvent;
	userEvent.type = SDL_USEREVENT;
	timerPushEvent.type = SDL_USEREVENT;
	timerPushEvent.user = userEvent;

	//find the player from the obj list
	player = findPlayer( myMap);
	if( ! player)
		quit( "No player was detected. You need a player object in the map.");

	playerVisibleTiles = (enum terrainType**) calloc( PLAYER_FOV_TILES_LIM, sizeof( enum terrainType*));
	int i;
	for( i=0; i< PLAYER_FOV_TILES_LIM; i++)
		playerVisibleTiles[i] = (enum terrainType*) calloc( PLAYER_FOV_TILES_LIM, sizeof( enum terrainType));

	init_fovBase( VIEW_RANGE);
}


int main( int argc, char *args[]) {
	//Default values
	myMap = 0;
	char *mapPath = 0;

	//Handle cmd-line arguments
	int argi = 1;
	while(argi < argc) {
		if( args[argi][0] == '-') {
			char c = args[argi][1];
			switch (c) {
				case 'm':
					mapPath = args[argi+1];
					argi++;
					break;
				default:
					argi = argc; //break out from the while-loop
					break;
			};
			argi++;
		}
		else {
			fprintf(stderr, "Unrecognized argument at index %d: \"%s\"", argi, args[argi]);
			exit(1);
		}
	}

	if(myMap == 0) {
		if(mapPath == 0) {
			fprintf( stderr, "There is no map. Open a map with '-m map-path'\n");
			exit(0);
		}
		else
			myMap = readMapFile( mapPath);
	}

	setDefaults();
	init();

	//FIXME temporary placement here. need their default values set somewhere better, maybe
	//vectorAdd( &playerMoveAreaStart, &viewPos, &PLAYER_PADDING_VECTOR );
	//vectorSub( &playerMoveAreaEnd, &viewEnd, &PLAYER_PADDING_VECTOR );

	textures = loadAllTextures( renderer);

	log0("All set and ready\nStarting...\n");

	running = 1;
	run();

	log0("Program over\nPeace\n");
	return 0;
}
