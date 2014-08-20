#include "engine.h"
#include "brush.h"
#include "aiTable.h"

#include "sdl2gui/sdl2gui.h"
#include "sdl2gui/sdl2gui-list.h"
#include "stack.h"



/* The drawing brushes. These are used to draw terrain and objects on map */
brushFun *brush; //draws something on the given location
int brushVariant;
struct brushState* myBrushState;


bool moveForward( struct Map *map, struct object* obj) { return false; }
bool turnLeft( struct Map *map, struct object *obj) { return false; }
bool turnRight( struct Map *map, struct object* obj) { return false; }

struct object *player;

/* GUI Elements */
#define GUI_LEFTPANEL_WIDTH 192
struct SDLGUI_Element *bodyContainer;
struct SDLGUI_Element *brushContainer;
struct SDLGUI_List *brushList;
bool mouseDownInGui;



/* -----
*/

bool defaultBrush( unsigned int x, unsigned int y, int type) {
	return false;
}

bool eraseObject( unsigned int x, unsigned int y, int type) {
	struct object *obj = myMap->objs[x][y];
	if( obj != NULL && obj->ai == NULL) {
		obj->isDeleted = true;
		if( obj->type == go_player)
			player = NULL;
		myMap->objs[x][y] = NULL;
		return true;
	}
	else
		return false;
}

bool eraseAI( unsigned int x, unsigned int y, int type) {
	struct object *obj = myMap->objs[x][y];
	if( obj != NULL && obj->ai != NULL) {
		AI_DESTROY( obj->ai);
		obj->ai = NULL;
		return true;
	}
	else
		return false;
}

bool drawTerrain( unsigned int x, unsigned int y, int type){
	if( myMap->tiles[x][y] != type) {
		myMap->tiles[x][y] = type;
		return true;
	}
	else
		return false;
}


void drawBackground() {
	log3("generate background\n");
	SDL_DestroyTexture( bgroundTexture);
	bgroundTexture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, viewSize.i*TILELEN, viewSize.j*TILELEN) ;
	SDL_SetRenderTarget( renderer, bgroundTexture);

	drawTexture( renderer, textures->trn[TEXTURE_TRN_NONE], 0, 0, windowW, windowH);

	unsigned int x,y,rx,ry;
	for( x=0, rx=viewPos.i; rx< MIN(myMap->width, viewEnd.i); x++, rx++)
    	for( y=0, ry=viewPos.j; ry< MIN(myMap->height, viewEnd.j); y++, ry ++)
    		drawTexture( renderer, textures->trn[ myMap->tiles[rx][ry] ],
    			x*TILELEN + GUI_LEFTPANEL_WIDTH, y*TILELEN, TILELEN, TILELEN
    			);

	SDL_SetRenderTarget( renderer, 0); //reset
}

bool drawObject( unsigned int x, unsigned int y, int type){
	//add objects only if there is no object there
	if( myMap->objs[x][y]!=0 || myMap->tiles[x][y]!=terrain_gnd)
		return false;

	//create and initialize a monster
	struct object *obj = createObject( type, x, y);

	addObject( obj, myMap, x, y);

	return true;
}

bool drawPlayer( unsigned int x, unsigned int y, int type) {
	//don't do anything if a player exists already
	if( player==0 && drawObject( x, y, go_player)) {
		player = myMap->objs[x][y];
		return true;
	}
	else
		return false;
}

bool drawAI( unsigned int x, unsigned int y, int type) {
	//if there is an object at the given location, and it dsoen't have an AI
	if( myMap->objs[x][y] != 0) {
		if ( myMap->objs[x][y]->ai != NULL && myMap->objs[x][y]->ai->type != type) 
			AI_DESTROY( myMap->objs[x][y]->ai);

		struct AI *ai = AI_CREATE( type);
		myMap->objs[x][y]->ai = ai;

		return true;
	}

	return false;
}

bool setDirection( unsigned int x, unsigned int y, int type) {
	//if there is an object at the given location, and it dsoen't have an AI
	if( myMap->objs[x][y] != 0) {
		myMap->objs[x][y]->dir = (enum direction)type;
		return true;
	}
	else
		return false;
}

void levelEditor_quit() {
	running =0;
}
void levelEditor_save() {
	saveMap( myMap);
}

bool scrollScreen( enum direction dir) {
	bool canScroll = false;
	switch(dir) {
		case dir_up:
			if(viewPos.j > 0)
				canScroll = true;
			break;
		case dir_left:
			if(viewPos.i > 0)
				canScroll = true;
			break;
		case dir_right:
			if(viewEnd.i < myMap->width )
				canScroll = true;
			break;
		case dir_down:
			if(viewEnd.j < myMap->height )
				canScroll = true;
			break;
	};

	if(canScroll) {
		struct Vector *dirVector = &dirVectors[dir];
		vectorAdd( &viewPos, &viewPos, dirVector );
		vectorAdd( &viewEnd, &viewEnd, dirVector );

		log1("scrolled to [(%d,%d), (%d,%d)]\n", viewPos.i, viewPos.j, viewEnd.i, viewEnd.j);
		drawBackground();
	}

	return canScroll;
}


void handleKey( SDL_KeyboardEvent *e) {
	switch (e->keysym.sym) {
		case SDLK_s:
			levelEditor_save();
			break;
		case SDLK_q:
			levelEditor_quit();
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
		x = (e->x - GUI_LEFTPANEL_WIDTH) / TILELEN;
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


void run() {
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
						bodyContainer->rect.h = e.window.data2;
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
				if( SDLGUI_Handle_MouseDown( (SDL_MouseButtonEvent*)&e)) {
					log0("mouse down gui\n");
					mouseDownInGui = true;
				}
				else {
					log0("mouse down level\n");
					mouseDownInGui = false;
					if (handleMouse( (SDL_MouseButtonEvent*)&e, 0) ) {
						drawBackground();
						break;
					}
				}
				continue;
			case SDL_MOUSEBUTTONUP:
				if( mouseDownInGui) {
					log0("mouse up gui\n");
					SDLGUI_Handle_MouseUp( (SDL_MouseButtonEvent*)&e);
				}
				break;
			case SDL_MOUSEMOTION:
				//TODO get this working again
				//if( handleMouse( 0, (SDL_MouseMotionEvent*)&e) ) {
				//	drawBackground();
				//	break;
				//}
				//else
				//	continue;
				continue;
			case SDL_KEYUP:
				/*don't do anything for those events*/
				continue;
			default:
				log1("unhandled event type: %d\n", e.type);
				continue;
		};
		draw();
	}
}

/* Draws the map and objects viewed on screen */
void drawObjects() {
	unsigned i;
	struct Vector screenPos;
	struct object *obj;

	struct object **newObjList = (struct object**)calloc( myMap->objListSize, sizeof( struct object*));
	int newCount = 0;

	for( i=0; i<myMap->objListCount; i++) {
		obj = myMap->objList[i];
		if( ! obj->isDeleted) {
			newObjList[ newCount] = obj;
			newCount ++;

			vectorSub( &screenPos, &myMap->objList[i]->pos, &viewPos );
			if( screenPos.i>=0 && screenPos.j>=0 && screenPos.i<viewSize.i && screenPos.j<viewSize.j ) {
				log3("drawing object %d\n", i);
				drawTexture( renderer, 
					textures->obj[obj->type]->textures[ obj->visualState][obj->dir], 
					screenPos.i*TILELEN + GUI_LEFTPANEL_WIDTH, screenPos.j*TILELEN, TILELEN, TILELEN );
			}
		}
	}

	free( myMap->objList);
	myMap->objList = newObjList;
	myMap->objListCount = newCount;
}


void draw() {
	log3("draw\n");
	SDL_RenderClear( renderer);
	//render background
	drawTexture( renderer, bgroundTexture, 0, 0, viewSize.i*TILELEN, viewSize.j*TILELEN);
	//render objects
	drawObjects( );

	SDLGUI_Draw();

	SDL_RenderPresent( renderer);

}

void buttonQuit_clicked( struct SDLGUI_Element *from) {
	levelEditor_quit();
}

void buttonSave_clicked( struct SDLGUI_Element *from) {
	log0("save button clicked\n");
	levelEditor_save();
}

struct brushWrapper {
	SDL_Keycode key;
	brushFun *brush;
	int brushVariant;
	struct SDLGUI_List *children;
};

struct SDLGUI_List *brushStack[4];
int brushStackCount = 0;

struct brushWrapper* CREATE_BRUSH_WRAPPER( SDL_Keycode key, brushFun *brush, int brushVariant, struct SDLGUI_List *children) {
	struct brushWrapper *result = (struct brushWrapper*)malloc( sizeof( struct brushWrapper));
	result->key = key;
	result->brush = brush;
	result->brushVariant = brushVariant;
	result->children = children;
	return result;
}

void listItem_clicked( struct SDLGUI_Element *from) {
	log0("list item clicked\n");

	assert(from->userData != NULL);
	struct brushWrapper *tmp = (struct brushWrapper*)from->userData;

	if( tmp->brush) {
		brush = tmp->brush;
		brushVariant = tmp->brushVariant;
	}
	if( tmp->children) {
		log0("changing list\n");
		STACK_PUSH( brushStack, brushList, brushStackCount);
		brushList = tmp->children;
		SDLGUI_Set_Panel_Elements( brushContainer, brushList, false);
	}
}

void brushBack_clicked( struct SDLGUI_Element *from) {
	if( STACK_IS_EMPTY( brushStackCount) != true) {
		STACK_POP( brushStack, brushList, brushStackCount);
		SDLGUI_Set_Panel_Elements( brushContainer, brushList, false);
	}
}

#define CREATE_LIST_BUTTON( i, text, data) SDLGUI_Create_Text( 5, 5 + 35*i, 160, 30, &listItem_clicked, text, (int[4]){255,255,255,255}, (int[4]){0,0,0,255}, 12, 20, 1, data)
void initGui() {
	SDLGUI_Init( renderer, textures->font);
	bodyContainer = SDLGUI_Create_Panel( 0, 0, GUI_LEFTPANEL_WIDTH, 960, (int[4]){170,180,190,255}, (int[4]){100,100,100,255}, 4);
	SDLGUI_Add_Element( bodyContainer);
	
	struct SDLGUI_List *bodyItems = SDLGUI_Get_Panel_Elements( bodyContainer);
	SDLGUI_List_Add( bodyItems, SDLGUI_Create_Text( 10, 50, GUI_LEFTPANEL_WIDTH - 2*10, 30, &buttonSave_clicked, "(s)ave", (int[4]){0,0,0,0}, (int[4]){0,0,0,255}, 12, 20, 1, NULL));
	SDLGUI_List_Add( bodyItems, SDLGUI_Create_Text( 10, 90, GUI_LEFTPANEL_WIDTH - 2*10, 30, &buttonQuit_clicked, "(q)uit", (int[4]){0,0,0,0}, (int[4]){0,0,0,255}, 12, 20, 1, NULL));

	SDLGUI_List_Add( bodyItems, SDLGUI_Create_Text( 10, 200, GUI_LEFTPANEL_WIDTH - 2*10, 30, &brushBack_clicked, "(Esc) Back", (int[4]){0,0,0,0}, (int[4]){0,0,0,255}, 12, 20, 1, NULL));

	brushContainer = SDLGUI_Create_Panel( 10, 240, GUI_LEFTPANEL_WIDTH - 2*10, 300, (int[4]){0,0,0,255}, (int[4]){255,255,255,255}, 1);
	SDLGUI_List_Add( bodyItems, brushContainer);

	brushList = SDLGUI_List_Create_From_Array(
		(struct SDLGUI_Element*[]){
			CREATE_LIST_BUTTON( 0, "rotate", CREATE_BRUSH_WRAPPER(/*key*/SDLK_1, /*brushFun*/&setDirection, /*brushVariant*/0, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "(1) up"		, CREATE_BRUSH_WRAPPER(/*key*/SDLK_1, /*brushFun*/&setDirection, /*brushVariant*/dir_up, /*children*/NULL)),
					CREATE_LIST_BUTTON( 1, "(2) right"	, CREATE_BRUSH_WRAPPER(/*key*/SDLK_2, /*brushFun*/&setDirection, /*brushVariant*/dir_right, /*children*/NULL)),
					CREATE_LIST_BUTTON( 2, "(3) down"	, CREATE_BRUSH_WRAPPER(/*key*/SDLK_3, /*brushFun*/&setDirection, /*brushVariant*/dir_down, /*children*/NULL)),
					CREATE_LIST_BUTTON( 3, "(4) left"	, CREATE_BRUSH_WRAPPER(/*key*/SDLK_4, /*brushFun*/&setDirection, /*brushVariant*/dir_left, /*children*/NULL)),
				}, 4
			))),
			CREATE_LIST_BUTTON( 1, "terrain", CREATE_BRUSH_WRAPPER(/*key*/SDLK_2, /*brushFun*/NULL, /*brushVariant*/0, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "(1) ground"	, CREATE_BRUSH_WRAPPER(/*key*/SDLK_1, /*brushFun*/&drawTerrain, /*brushVariant*/terrain_gnd, /*children*/NULL)),
					CREATE_LIST_BUTTON( 1, "(2) wall"	, CREATE_BRUSH_WRAPPER(/*key*/SDLK_2, /*brushFun*/&drawTerrain, /*brushVariant*/terrain_wall, /*children*/NULL)),
				}, 2
			)))
		}, 2
	);
	SDLGUI_Set_Panel_Elements( brushContainer, brushList, true);
	
}
#undef CREATE_LIST_BUTTON


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
		log0( "reading map file %s\n", mapPath);
		myMap = readMapFile( mapPath);
	}

	player = findPlayer( myMap);

	init();

    log0("loading textures\n");
	textures = loadAllTextures( renderer);
	myBrushState = &initialBrushState;
	brush = defaultBrush;

	initGui();

	log0("All set and ready\nStarting...\n");

	running = 1;
	run();

	log0("Program over\nPeace\n");
	SDLGUI_Destroy();
	SDL_DestroyRenderer( renderer);
	SDL_DestroyWindow( window);
	SDL_Quit();
	return 0;
}
