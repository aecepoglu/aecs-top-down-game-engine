#include "engine.h"
#include "aiTable.h"

SDL_Event timerPushEvent;
Uint32 timerDelay;

struct object *player;

/* The player is allowed to play only once per tick. This variable shows whether the player has moved or not */
bool playerMoved = false;

#define VIEW_RANGE 3  //TODO Use FOV distance instead of '10'
struct Vector playerMoveAreaStart, playerMoveAreaEnd;
struct Vector PLAYER_PADDING_VECTOR = {VIEW_RANGE, VIEW_RANGE};
//FIXME The program will crash if the window is too small. Render window un-usable and show a notification message about it.

void gameOver() {
	log0("game over...\n");
	running=false;
}

/* Moves forward only if that spot is empty */
bool moveBackward( struct Map *map, struct object* obj) {
	struct Vector newPos;
	vectorSub( &dirVectors[ obj->dir ], &newPos, &obj->pos);
	if( map->tiles[newPos.i][newPos.j] == 0 && map->objs[newPos.i][newPos.j] == 0) {
		map->objs[ obj->pos.i ][ obj->pos.j ] = 0;
		map->objs[ newPos.i ][ newPos.j ] = obj;
		vectorClone( &newPos, &obj->pos);
		return true;
	}
	else {
		return false;
	}
}

/* Move forward. Hit the object if can't move forward */
bool moveForward( struct Map *map, struct object* obj) {
	struct Vector newPos;
	vectorAdd( &dirVectors[ obj->dir ], &obj->pos, &newPos );
	if( map->tiles[newPos.i][newPos.j] == 0 ) {
		struct object *objAtPos = map->objs[newPos.i][newPos.j];
		if( ! objAtPos ) {
			map->objs[ obj->pos.i ][ obj->pos.j ] = 0;
			map->objs[ newPos.i ][ newPos.j ] = obj;
			vectorClone( &newPos, &obj->pos);
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
					else {
						
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
	obj->dir = (obj->dir - 1) % 4;
	return true;
}

bool turnRight( struct Map *map, struct object *obj) {
	obj->dir = (obj->dir + 1) % 4;
	return true;
}

bool eat( struct Map *map, struct object *obj) {
	struct Vector newPos;
	vectorAdd( &dirVectors[ obj->dir ], &obj->pos, &newPos);
	struct object *otherObj = map->objs[newPos.i][newPos.j];
	if( otherObj != 0 && otherObj->health == 0) {
		objectSwallow( obj, otherObj);
		map->objs[ newPos.i ][ newPos.j ] = 0;
		/* mark it for 'awaiting deletion'
		It will be deleted in the next update cycle */
		otherObj->isDeleted=true;
		if( otherObj == player) {
			gameOver();
		}
	}
}

void movePlayer( bool (moveFunction)(struct Map*, struct object*) ) {
	if( ! playerMoved) {
		playerMoved = true;
		if ( moveFunction( myMap, player) ) {
			//scroll if necessary

			bool needsScroll = true;
			enum direction scrollDirection;

			if( player->pos.i >= playerMoveAreaEnd.i && scrollScreen(dir_right))
				scrollDirection = dir_right;
			else if( player->pos.i < playerMoveAreaStart.i && scrollScreen(dir_left))
				scrollDirection = dir_left;
			else if( player->pos.j >= playerMoveAreaEnd.j && scrollScreen(dir_down))
				scrollDirection = dir_down;
			else if( player->pos.j < playerMoveAreaStart.j && scrollScreen(dir_up))
				scrollDirection = dir_up;
			else
				needsScroll = false;

			if(needsScroll) {
				struct Vector *scrollVector = &dirVectors[scrollDirection];
				vectorAdd( scrollVector, &playerMoveAreaStart, &playerMoveAreaStart);
				vectorAdd( scrollVector, &playerMoveAreaEnd, &playerMoveAreaEnd);
			}
				
		}
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
	log2("tick\n");
	SDL_PushEvent( &timerPushEvent);
	return interval;
}

void update() {
	log2("update\n");
	unsigned int i;
	for( i=0; i<myMap->objListCount; i++) {
		//update object at [i]
		/* Some objects are marked for deletion
		Copy all non-deleted objects into a new-list
		and that list is the new object-list */
		if( myMap->objList[i]->isDeleted ) {
			//TODO implement obj deletion. Just not showing the deleted objects now
			//TODO don't forget to remove the matching code from draw()
		}
		else if( myMap->objList[i]->ai ) {
			if( myMap->objList[i]->timerCounter == 0) {
				AI_UPDATE( myMap, myMap->objList[i] );
			}
			myMap->objList[i]->timerCounter --;
		}
	}
	playerMoved = false;
}

int run() {
    drawBackground();
	draw(); //the initial draw //TODO check if necessary

	SDL_AddTimer( timerDelay, timerCallback, 0);

	SDL_Event e;
	while( running) {
		SDL_WaitEvent( &e);
		switch (e.type) {
			case SDL_WINDOWEVENT:
				switch( e.window.event) {
					case SDL_WINDOWEVENT_SHOWN:
					    log1("Window %d shown\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_HIDDEN:
					    log1("Window %d hidden\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_EXPOSED:
					    //log1("Window %d exposed\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_RESIZED:
					    log1("Window %d resized to %dx%d\n", e.window.windowID, e.window.data1, e.window.data2);
						resizeView(e.window.data1, e.window.data2);
						vectorAdd( &viewPos, &PLAYER_PADDING_VECTOR, &playerMoveAreaStart);
						vectorSub( &viewEnd, &PLAYER_PADDING_VECTOR, &playerMoveAreaEnd);

						drawBackground();
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
	textures = loadTextures( renderer);

	log0("All set and ready\nStarting...\n");

	running = 1;
	run();

	log0("Program over\nPeace\n");
	return 0;
}
