#include "engine.h"
#include "aiTable.h"

SDL_Event timerPushEvent;
Uint32 timerDelay;

struct object *player;

/* The player is allowed to play only once per tick. This variable shows whether the player has moved or not */
bool playerMoved = false;

#define VIEW_RANGE 10  //TODO Use FOV distance instead of '10'
int playerViewLimXMin = VIEW_RANGE;
int playerViewLimYMin = VIEW_RANGE;
int playerViewLimXMax;
int playerViewLimYMax;
//FIXME The program will crash if the window is too small. Render window un-usable and show a notification message about it.

bool moveBackward( struct Map *map, struct object* obj) {
	struct Vector newPos = { obj->pos.i, obj->pos.j};
	vectorSub( &newPos, &dirVectors[ obj->dir ] );
	if( myMap->tiles[newPos.i][newPos.j] == terrain_none
		&& myMap->objs [newPos.i][newPos.j] == 0 )
	{
		obj->pos.i = newPos.i;
		obj->pos.j = newPos.j;
		return true;
	}
	else
		return false;
}

bool moveForward( struct Map *map, struct object* obj) {
	struct Vector newPos = { obj->pos.i, obj->pos.j};
	vectorAdd( &newPos, &dirVectors[ obj->dir ]);
	if( myMap->tiles[newPos.i][newPos.j] == terrain_none
		&& myMap->objs [newPos.i][newPos.j] == 0 )
	{
		obj->pos.i = newPos.i;
		obj->pos.j = newPos.j;
		return true;
	}
	else
		return false;

}

bool turnLeft( struct Map *map, struct object *obj) {
	obj->dir = (obj->dir - 1) % 4;
	return true;
}

bool turnRight( struct Map *map, struct object *obj) {
	obj->dir = (obj->dir + 1) % 4;
	return true;
}

void movePlayer( bool (moveFunction)(struct Map*, struct object*) ) {
	if( ! playerMoved) {
		playerMoved = true;
		if ( moveFunction( myMap, player) ) {
			//scroll if necessary
			if( player->pos.i > playerViewLimXMax)
				scrollScreen( 1, 0);
			else if ( player->pos.i < playerViewLimXMin)
				scrollScreen( -1, 0);
			else if ( player->pos.j > playerViewLimYMax)
				scrollScreen( 0, 1);
			else if ( player->pos.j < playerViewLimYMin)
				scrollScreen( 0, -1);
		}
	}
}

void handleKey( SDL_KeyboardEvent *e) {
	switch (e->keysym.sym) {
		case SDLK_q:
			quit("Quit-key pressed.\n");
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
		default:
			log0("Unhandled key\n");
			break;
	};
}

Uint32 timerCallback( Uint32 interval, void *param) {
	log0("tick\n");
	SDL_PushEvent( &timerPushEvent);
	return interval;
}

void update() {
	log0("update\n");
	unsigned int i;
	for( i=0; i<myMap->objListCount; i++) {
		//update object at [i]
		if( myMap->objList[i]->ai )
			AI_UPDATE( myMap, myMap->objList[i] );
	}
	playerMoved = false;
}

int run() {
    drawBackground();
	draw(); //the initial draw //TODO check if necessary

	SDL_AddTimer( timerDelay, timerCallback, 0);

	SDL_Event e;
	while( running) {
		//while( SDL_PollEvent( &e)) {
		//}
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
					    log0("Window %d resized to %dx%d\n", e.window.windowID, e.window.data1, e.window.data2);
						windowW = (e.window.data1-1) / TILELEN;
						windowH = (e.window.data2-1) / TILELEN;
						playerViewLimXMax = windowW - VIEW_RANGE;
						playerViewLimYMax = windowH - VIEW_RANGE;

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

	timerDelay = 1000;

	SDL_UserEvent userEvent;
	userEvent.type = SDL_USEREVENT;
	timerPushEvent.type = SDL_USEREVENT;
	timerPushEvent.user = userEvent;

	//find the player from the obj list
	int i;
	for( i=0; i<myMap->objListCount; i++) {
		if( myMap->objList[i]->type == go_player ) {
			player = myMap->objList[i];
			break;
		}
	}
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
