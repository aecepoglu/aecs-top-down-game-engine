#include "engine.h"
#include "brush.h"
#include "aiTable.h"

#include "sdl2gui/sdl2gui.h"
#include "sdl2gui/sdl2gui-list.h"
#include "stack.h"

#include "error.h"



bool moveForward( struct Map *map, struct object* obj) { return false; }
bool turnLeft( struct Map *map, struct object *obj) { return false; }
bool turnRight( struct Map *map, struct object* obj) { return false; }

/* GUI Elements */
#define GUI_LEFTPANEL_WIDTH 192
struct SDLGUI_Element *bodyContainer;
struct SDLGUI_Element *brushContainer;
struct SDLGUI_List *brushList;
bool mouseDownInGui;
bool isMessageBoxOn = false;

struct SDLGUI_List *brushStack[4];
int brushStackCount = 0;

struct brushWrapper {
	SDL_Keycode key;
	brushFun *brush;
	int brushVariant;
	struct SDLGUI_List *children;
};


/* Brush vars */
brushFun *brush = NULL; //draws something on the given location
int brushVariant;



/* ---------------------------
	Brush Functions
*/

bool eraseObject( unsigned int x, unsigned int y, int type) {
	struct object *obj = myMap->objs[x][y];
	if( obj != NULL && obj->ai == NULL) {
		obj->isDeleted = true;
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
	if( myMap->tiles[x][y] != type && myMap->objs[x][y]==NULL) {
		myMap->tiles[x][y] = type;
		return true;
	}
	else
		return false;
}

bool drawObject( unsigned int x, unsigned int y, int type){
	//add objects only if there is no object there
	if( myMap->objs[x][y]!=NULL || myMap->tiles[x][y]!=terrain_gnd)
		return false;

	//create and initialize a monster
	struct object *obj = createObject( type, x, y);

	addObject( obj, myMap, x, y);

	return true;
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

/* ----------------
	TODO title
*/

void drawBackground() {
	log3("generate background\n");
	SDL_DestroyTexture( bgroundTexture);
	bgroundTexture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, viewSize.i*TILELEN, viewSize.j*TILELEN) ;
	SDL_SetRenderTarget( renderer, bgroundTexture);

	SDL_SetRenderDrawColor( renderer, 0,0,0, 255);
	SDL_RenderClear( renderer);

	unsigned int x,y,rx,ry;
	for( x=0, rx=viewPos.i; rx< MIN(myMap->width, viewEnd.i); x++, rx++)
    	for( y=0, ry=viewPos.j; ry< MIN(myMap->height, viewEnd.j); y++, ry ++)
    		drawTexture( renderer, textures->trn[ myMap->tiles[rx][ry] ],
    			x*TILELEN, y*TILELEN, TILELEN, TILELEN
    			);

	SDL_SetRenderTarget( renderer, 0); //reset
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

void changeBrush( struct brushWrapper *bw) {
	
	if( bw->brush) {
		brush = bw->brush;
		brushVariant = bw->brushVariant;
	}
	if( bw->children) {
		log0(" changing list\n");
		STACK_PUSH( brushStack, brushList, brushStackCount);
		brushList = bw->children;
		SDLGUI_Set_Panel_Elements( brushContainer, brushList, false);
	}
}



void selectBrushWithKeysym( SDL_Keycode key) {
	int i;
	for( i=0; i<brushList->count; i++) {
		struct brushWrapper *bw = (struct brushWrapper *)brushList->list[i]->userData;
		if( bw && key == bw->key) {
			changeBrush( bw);
			break;
		}
	}
}

/* gui callbacks
*/

void buttonQuit_clicked( struct SDLGUI_Element *from) {
	running =0;
}

void buttonSave_clicked( struct SDLGUI_Element *from) {
	enum SDLGUI_Message_Type msgType;
	char *msgText;
	if( saveMap( myMap) != true) {
		msgType = SDLGUI_MESSAGE_ERROR;
		msgText = errorMessage;
	}
	else {
		msgType = SDLGUI_MESSAGE_INFO;
		msgText = "Map saved";
	}
	isMessageBoxOn = true;
	SDLGUI_Show_Message(0, 0, windowW, windowH, msgType, msgText);
}

void brushBack_clicked( struct SDLGUI_Element *from) {
	if( STACK_IS_EMPTY( brushStackCount) != true) {
		STACK_POP( brushStack, brushList, brushStackCount);
		SDLGUI_Set_Panel_Elements( brushContainer, brushList, false);
	}
}

void brushListItem_clicked( struct SDLGUI_Element *from) {
	assert(from->userData != NULL);
	changeBrush( (struct brushWrapper*)from->userData);
}

void handleKey( SDL_KeyboardEvent *e) {
	log0("is repeat: %d\n", e->repeat);
	if( e->repeat) {
		switch( e->keysym.sym) {
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
		};
	}
	else {
		switch (e->keysym.sym) {
			case SDLK_s:
				buttonSave_clicked(NULL);
				break;
			case SDLK_q:
				buttonQuit_clicked(NULL);
				break;
			case SDLK_TAB:
				brushBack_clicked( NULL);
				break;
			default:
				if( e->keysym.sym >= SDLK_0 && e->keysym.sym <= SDLK_9)
					selectBrushWithKeysym( e->keysym.sym);
				break;
		};
	}
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
						resizeView( GUI_LEFTPANEL_WIDTH, e.window.data1, e.window.data2);
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
				if( isMessageBoxOn) {
					log0("msg box on\n");
					SDLGUI_Hide_Message();
					isMessageBoxOn =false;
					break;
				}
				handleKey( (SDL_KeyboardEvent*)&e);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if( isMessageBoxOn) {
					SDLGUI_Hide_Message();
					isMessageBoxOn =false;
					break;
				}
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
				if( isMessageBoxOn) {
					SDLGUI_Hide_Message();
					isMessageBoxOn =false;
					break;
				}
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
	drawTexture( renderer, bgroundTexture, GUI_LEFTPANEL_WIDTH, 0, viewSize.i*TILELEN, viewSize.j*TILELEN);
	//render objects
	drawObjects( );

	SDLGUI_Draw();

	SDL_RenderPresent( renderer);

}



struct brushWrapper* CREATE_BRUSH_WRAPPER( SDL_Keycode key, brushFun *brush, int brushVariant, struct SDLGUI_List *children) {
	struct brushWrapper *result = (struct brushWrapper*)malloc( sizeof( struct brushWrapper));
	result->key = key;
	result->brush = brush;
	result->brushVariant = brushVariant;
	result->children = children;
	return result;
}

#define NO_VAR 0
#define NO_CHILDREN NULL
#define NO_FUN NULL
#define CREATE_LIST_BUTTON( i, text, data) SDLGUI_Create_Text( 5, 5 + 35*i, 160, 30, &brushListItem_clicked, text, (int[4]){255,255,255,255}, (int[4]){0,0,0,255}, 6, 9, 1, data)
void initGui() {
	SDLGUI_Init( renderer, textures->font);
	bodyContainer = SDLGUI_Create_Panel( 0, 0, GUI_LEFTPANEL_WIDTH, 960, (int[4]){170,180,190,255}, (int[4]){100,100,100,255}, 4);
	SDLGUI_Add_Element( bodyContainer);
	
	struct SDLGUI_List *bodyItems = SDLGUI_Get_Panel_Elements( bodyContainer);
	SDLGUI_List_Add( bodyItems, SDLGUI_Create_Text( 10, 10, GUI_LEFTPANEL_WIDTH - 2*10, 70, &buttonSave_clicked, "YZ-01\nLevel Editor\n------------", (int[4]){0,0,0,0}, (int[4]){0,0,0,255}, 12, 20, 0, NULL));
	SDLGUI_List_Add( bodyItems, SDLGUI_Create_Text( 10, 70, GUI_LEFTPANEL_WIDTH - 2*10, 30, &buttonSave_clicked, "(s)ave", (int[4]){0,0,0,0}, (int[4]){0,0,0,255}, 12, 20, 1, NULL));
	SDLGUI_List_Add( bodyItems, SDLGUI_Create_Text( 10, 110, GUI_LEFTPANEL_WIDTH - 2*10, 30, &buttonQuit_clicked, "(q)uit", (int[4]){0,0,0,0}, (int[4]){0,0,0,255}, 12, 20, 1, NULL));

	SDLGUI_List_Add( bodyItems, SDLGUI_Create_Text( 10, 200, GUI_LEFTPANEL_WIDTH - 2*10, 30, &brushBack_clicked, "(Tab) Back", (int[4]){0,0,0,0}, (int[4]){0,0,0,255}, 12, 20, 1, NULL));

	brushContainer = SDLGUI_Create_Panel( 10, 240, GUI_LEFTPANEL_WIDTH - 2*10, 300, (int[4]){0,0,0,255}, (int[4]){255,255,255,255}, 1);
	SDLGUI_List_Add( bodyItems, brushContainer);

	/* Table readable with tab-width 4 */
	brushList = SDLGUI_List_Create_From_Array(
		(struct SDLGUI_Element*[]){
			CREATE_LIST_BUTTON( 0, "1. rotate", CREATE_BRUSH_WRAPPER( SDLK_1, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "(1) \x80 up"	, 				CREATE_BRUSH_WRAPPER( SDLK_1, &setDirection, dir_up, 		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "(2) \x81 right", 			CREATE_BRUSH_WRAPPER( SDLK_2, &setDirection, dir_right, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 2, "(3) \x82 down", 				CREATE_BRUSH_WRAPPER( SDLK_3, &setDirection, dir_down,		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 3, "(4) \x83 left", 				CREATE_BRUSH_WRAPPER( SDLK_4, &setDirection, dir_left,		NO_CHILDREN)),
				}, 4
			))),
			CREATE_LIST_BUTTON( 1, "2. terrain", CREATE_BRUSH_WRAPPER( SDLK_2, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "(1) ground", 			CREATE_BRUSH_WRAPPER( SDLK_1, &drawTerrain, terrain_gnd, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "(2) wall", 	 			CREATE_BRUSH_WRAPPER( SDLK_2, &drawTerrain, terrain_wall, 	NO_CHILDREN)),
				}, 2
			))),
			CREATE_LIST_BUTTON( 2, "3. erase", CREATE_BRUSH_WRAPPER(/*key*/SDLK_3, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "(1) object", 			CREATE_BRUSH_WRAPPER( SDLK_1, &eraseObject, NO_VAR, 		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "(2) ai",	 			CREATE_BRUSH_WRAPPER( SDLK_2, &eraseAI, 	NO_VAR, 		NO_CHILDREN)),
				}, 2
			))),
			CREATE_LIST_BUTTON( 3, "4. object", CREATE_BRUSH_WRAPPER( SDLK_4, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "(1) player", 			CREATE_BRUSH_WRAPPER( SDLK_1, &drawObject, go_player, 		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "(2) left turner", 		CREATE_BRUSH_WRAPPER( SDLK_2, &drawObject, go_leftTurner, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 2, "(3) apple", 			CREATE_BRUSH_WRAPPER( SDLK_3, &drawObject, go_apple, 		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 3, "(4) flower", 			CREATE_BRUSH_WRAPPER( SDLK_4, &drawObject, go_flower, 		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 4, "(5) creeper plant", 	CREATE_BRUSH_WRAPPER( SDLK_5, &drawObject, go_creeperPlant, NO_CHILDREN)),
					CREATE_LIST_BUTTON( 5, "(6) peekaboo monster", 	CREATE_BRUSH_WRAPPER( SDLK_6, &drawObject, go_peekaboo, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 6, "(7) weeping angel", 	CREATE_BRUSH_WRAPPER( SDLK_7, &drawObject, ai_weepingAngel, NO_CHILDREN)),
				}, 7
			))),
			CREATE_LIST_BUTTON( 4, "5. ai", CREATE_BRUSH_WRAPPER( SDLK_5, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "(1) left turner", 		CREATE_BRUSH_WRAPPER( SDLK_1, &drawAI, ai_leftTurner, 		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "(2) hungry left turner",CREATE_BRUSH_WRAPPER( SDLK_2, &drawAI, ai_hungryLeftTurner, NO_CHILDREN)),
					CREATE_LIST_BUTTON( 2, "(3) simple flower", 	CREATE_BRUSH_WRAPPER( SDLK_3, &drawAI, ai_simpleFlower, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 3, "(4) creeper plant", 	CREATE_BRUSH_WRAPPER( SDLK_4, &drawAI, ai_creeperPlant, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 4, "(5) peek-a-boo chaser", CREATE_BRUSH_WRAPPER( SDLK_5, &drawAI, ai_peekaboo, 		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 5, "(6) weeping angel", 	CREATE_BRUSH_WRAPPER( SDLK_6, &drawAI, ai_weepingAngel, 	NO_CHILDREN)),
				}, 6
			))),
		}, 5
	);
	SDLGUI_Set_Panel_Elements( brushContainer, brushList, true);
	
}
#undef CREATE_LIST_BUTTON
#undef NO_VAR
#undef NO_CHILDREN
#undef NO_FUN

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

	init();

    log0("loading textures\n");
	textures = loadAllTextures( renderer);

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
