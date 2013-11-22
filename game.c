#include "engine.h"
#include "aiTable.h"

SDL_Event timerPushEvent;
Uint32 timerDelay;

bool moveForward( struct Map *map, struct object* obj) {
	struct Vector newPos = { obj->pos.i, obj->pos.j};
	vectorAdd( &newPos, &dirVectors[ obj->dir]);
	if( IS_VECTOR_IN_REGION( newPos, 0, 0, myMap->width, myMap->height )
		&& myMap->tiles[newPos.i][newPos.j] == terrain_none
		&& myMap->objs [newPos.i][newPos.j] == 0 ) {
		obj->pos.i = newPos.i;
		obj->pos.j = newPos.j;
		return 1;
	}
	else
		return 0;

}
bool turnLeft( struct Map *map, struct object *obj) {
	vectorRotate( obj->dir, true);
}
bool turnRight( struct Map *map, struct object *obj) {
	vectorRotate( obj->dir, false);
}


void handleKey( SDL_KeyboardEvent *e) {
	switch (e->keysym.sym) {
		case SDLK_q:
			quit("Quit-key pressed.\n");
			break;
		default:
			log0("Unhandled key\n");
			break;
/* TODO Disabling the scroll controls for now. These keys should move the player around the map, and scroll the map as necessary.
		case SDLK_UP:
			scrollScreen( 0, -1);
			break;
		case SDLK_DOWN:
			scrollScreen( 0, +1);
			break;
		case SDLK_RIGHT:
			scrollScreen( +1, 0);
			break;
		case SDLK_LEFT:
			scrollScreen( -1, 0);
			break;
			*/
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

	timerDelay = 500;

	SDL_UserEvent userEvent;
	userEvent.type = SDL_USEREVENT;
	timerPushEvent.type = SDL_USEREVENT;
	timerPushEvent.user = userEvent;
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
