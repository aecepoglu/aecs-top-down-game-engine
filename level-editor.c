#include "engine.h"
#include "brush.h"
#include "aiTable.h"
#include "array.h"


/* The drawing brushes. These are used to draw terrain and objects on map */
brushFun *brush; //draws something on the given location
int brushVariant;
struct brushState* myBrushState;


bool moveForward( struct Map *map, struct object* obj) { }
bool turnLeft( struct Map *map, struct object *obj) { }

struct object *player;


bool defaultBrush( unsigned int x, unsigned int y, int type) {
	return 0;
}

bool drawTerrain( unsigned int x, unsigned int y, int type){
	if( myMap->tiles[x][y] != type) {
		myMap->tiles[x][y] = type;
		return 1;
	}
	else
		return 0;
}

bool drawObject( unsigned int x, unsigned int y, int type){
	//add objects only if there is no object there
	if( myMap->objs[x][y]!=0 || myMap->tiles[x][y]!=terrain_none)
		return 0;

	//create and initialize a monster
	struct object *obj = createObject( type, x, y);

	ARRAY_ADD( myMap->objList, obj, myMap->objListCount, myMap->objListSize, sizeof( struct object));

	//the 2d objs array maps to actual object objects
	myMap->objs[x][y] = obj;

	return 1;
}

bool drawPlayer( unsigned int x, unsigned int y, int type) {
	//don't do anything if a player exists already
	if( player==0 && drawObject( x, y, go_player)) {
		player = myMap->objs[x][y];
		return 1;
	}
	else
		return 0;
}

bool drawAI( unsigned int x, unsigned int y, int type) {
	//if there is an object at the given location, and it dsoen't have an AI
	if( myMap->objs[x][y] != 0 && myMap->objs[x][y]->ai==0) {
		struct AI *ai = AI_CREATE( type);
		myMap->objs[x][y]->ai = ai;
		return 1;
	}

	return 0;
}

bool setDirection( unsigned int x, unsigned int y, int type) {
	//if there is an object at the given location, and it dsoen't have an AI
	if( myMap->objs[x][y] != 0) {
		myMap->objs[x][y]->dir = (enum direction)type;
		return 1;
	}

	return 0;
}

void handleKey( SDL_KeyboardEvent *e) {
	switch (e->keysym.sym) {
		case SDLK_s:
			/* saves the map, and continues */
			saveMap( myMap);
			log0("saved\n");
			break;
		case SDLK_q:
			/* terminates program without saving anything */
			running =0;
			break;

		case SDLK_UP:
			scrollScreen( dir_up);
			break;
		case SDLK_DOWN:
			scrollScreen( dir_down);
			break;
		case SDLK_RIGHT:
			scrollScreen( dir_right);
			break;
		case SDLK_LEFT:
			scrollScreen( dir_left);
			break;
		default:
			if( e->keysym.sym >= SDLK_0 && e->keysym.sym <= SDLK_9)
				brush = updateBrushState( &myBrushState, &brushVariant, e->keysym.sym);
			break;
	};
}

/* The mouse button-event and motion-events are handled the same
 * I just need to extract the x,y from the event-data.
 */
bool handleMouse( SDL_MouseButtonEvent *e, SDL_MouseMotionEvent *e2) {
	unsigned int x,y;
	if( e) {
		x = e->x / TILELEN;
		y = e->y / TILELEN;
	}
	else {
		if( ! e2->state) //if no buttons are pressed
			return 0;
		x = e2->x / TILELEN;
		y = e2->y / TILELEN;
	}

	x += viewPos.i;
	y += viewPos.j;

	return ( x < myMap->width && y < myMap->height && brush( x, y, brushVariant) );
}


int run() {
    drawBackground();
	draw();
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
					    log1("Window %d resized to %dx%d\n",
					            e.window.windowID, e.window.data1,
					            e.window.data2);
						resizeView(e.window.data1, e.window.data2);
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
			case SDL_QUIT:
				quit("Quitting");
				running =0;
				break;
			case SDL_KEYDOWN:
				log1("key down\n");
				handleKey( (SDL_KeyboardEvent*)&e);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (handleMouse( (SDL_MouseButtonEvent*)&e, 0) ) {
					drawBackground();
					break;
				}
				else
					continue;
				break;
			case SDL_MOUSEMOTION:
				if( handleMouse( 0, (SDL_MouseMotionEvent*)&e) ) {
					drawBackground();
					break;
				}
				else
					continue;
			case SDL_KEYUP:
			case SDL_MOUSEBUTTONUP:
				/*don't do anything for those events*/
				continue;
			default:
				log1("unhandled event type: %d\n", e.type);
				continue;
		};
		draw();
	}
}




int main( int argc, char *args[]) {
	//Default values
	myMap = 0;
	char *mapPath = 0;
	player = 0;

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
				case 'n':
					myMap = createNewMap( (unsigned int)atoi(args[argi+1]), (unsigned int)atoi(args[argi+2]));
					myMap->filePath = mapPath;
					argi+=2;
					break;
				default:
					argi = argc; //break out from the loop
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
			fprintf( stderr, "There is no map. Open a map with '-m map-path' or create new map with '-m map-path -n map-width map-height'\n");
			exit(0);
		}
		else
			log1( "reading map file %s\n", mapPath);
			log0( "reading map file %s\n", mapPath);
			myMap = readMapFile( mapPath);
	}

	player = findPlayer( myMap);

	init();

    log0("loading textures\n");
	textures = loadTextures( renderer);
	myBrushState = &initialBrushState;
	brush = defaultBrush;

	log0("All set and ready\nStarting...\n");

	running = 1;
	run();

	log0("Program over\nPeace\n");
	SDL_DestroyRenderer( renderer);
	SDL_DestroyWindow( window);
	SDL_Quit();
	return 0;
}
