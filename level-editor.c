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

struct SDLGUI_Element *textbox_health;
struct SDLGUI_Element *textbox_maxHealth;
struct object *selectedObj = NULL;

struct {
	struct SDLGUI_Element *panel, *textbox_width, *textbox_height;
	bool running;
} createMapDialogData;


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
		
		SDLGUI_Show_Tooltip( x*TILELEN + GUI_LEFTPANEL_WIDTH, y*TILELEN, "AI removed");

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

bool editor_createObj( unsigned int x, unsigned int y, int type){
	//add objects only if there is no object there
	if( myMap->objs[x][y]!=NULL || myMap->tiles[x][y]!=terrain_gnd)
		return false;

	//create and initialize a monster
	struct object *obj = createObject( type, x, y);

	addObject( obj, myMap, x, y);

	return true;
}

bool editor_changeObjType( unsigned int x, unsigned int y, int type) {
	if( myMap->objs[x][y]==NULL)
		return false;
	
	myMap->objs[x][y]->type = type;
	return true;
}

bool editor_selectObj( unsigned int x, unsigned int y, int type) {
	
	selectedObj = myMap->objs[x][y];
	
	if( selectedObj == NULL)
		return false;

	char tmp[4];

	sprintf(tmp, "%d", selectedObj->health);
	SDLGUI_SetText_Textbox( textbox_health, strdup(tmp));

	sprintf(tmp, "%d", selectedObj->maxHealth);
	SDLGUI_SetText_Textbox( textbox_maxHealth, strdup(tmp));

	return true;
}

bool drawAI( unsigned int x, unsigned int y, int type) {
	//if there is an object at the given location, and it dsoen't have an AI
	if( myMap->objs[x][y] != 0) {
		if ( myMap->objs[x][y]->ai != NULL && myMap->objs[x][y]->ai->type != type) 
			AI_DESTROY( myMap->objs[x][y]->ai);

		struct AI *ai = AI_CREATE( type);
		myMap->objs[x][y]->ai = ai;

		SDLGUI_Show_Tooltip( x*TILELEN + GUI_LEFTPANEL_WIDTH, y*TILELEN, "AI put");
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
	running =false;
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

int parseText( const char *text) {
	if( strlen(text) == 0)
		return 0;
	else
		return atoi( text);
}

void textbox_health_changed( struct SDLGUI_Element *textbox, const char *text) {
	if( selectedObj) {
		int newHealth = parseText( text);
		if( newHealth < 256) {
			selectedObj->health = newHealth;
		}
		else {
			SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_ERROR, "Health should be less than 256");
			isMessageBoxOn = true;
		}
	}
	else {
		SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_WARNING, "No object selected");
		isMessageBoxOn = true;
	}
}

void textbox_maxHealth_changed( struct SDLGUI_Element *textbox, const char *text) {
	if( selectedObj) {
		int newHealth = parseText( text);
		if( newHealth < 256) {
			selectedObj->maxHealth = newHealth;
		}
		else {
			SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_ERROR, "Max-Health should be less than 256");
			isMessageBoxOn = true;
		}
	}
	else {
		SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_WARNING, "No object selected");
		isMessageBoxOn = true;
	}
}

void handleKey( SDL_KeyboardEvent *e) {
	bool matched = e->repeat != true;
	if( e->repeat != true) {
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
				if( e->keysym.sym >= SDLK_0 && e->keysym.sym <= SDLK_9) {
					matched = true;
					selectBrushWithKeysym( e->keysym.sym);
				}
                else
				    matched = false;
				break;
		};
	}

	if( matched != true) {
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
		x = (e2->x - GUI_LEFTPANEL_WIDTH) / TILELEN;
		y = e2->y / TILELEN;
	}

	x += viewPos.i;
	y += viewPos.j;

	return ( brush != NULL && x < myMap->width && y < myMap->height && brush( x, y, brushVariant) );
}

void handleWindowEvent( SDL_WindowEvent *e) {
	switch( e->event) {
		case SDL_WINDOWEVENT_SHOWN:
		    log1("Window %d shown\n", e->windowID);
		    break;
		case SDL_WINDOWEVENT_HIDDEN:
		    log1("Window %d hidden\n", e->windowID);
		    break;
		case SDL_WINDOWEVENT_EXPOSED:
		    //log1("Window %d exposed\n", e->windowID);
		    break;
		case SDL_WINDOWEVENT_RESIZED:
		    log1("Window %d resized to %dx%d\n",
		            e->windowID, e->data1,
		            e->data2);
			resizeView( GUI_LEFTPANEL_WIDTH, e->data1, e->data2);
			bodyContainer->rect.h = e->data2;
			drawBackground();
		    break;
		case SDL_WINDOWEVENT_MINIMIZED:
		    log1("Window %d minimized\n", e->windowID);
		    break;
		case SDL_WINDOWEVENT_MAXIMIZED:
		    log1("Window %d maximized\n", e->windowID);
		    break;
		case SDL_WINDOWEVENT_RESTORED:
		    log1("Window %d restored\n", e->windowID);
		    break;
		case SDL_WINDOWEVENT_CLOSE:
		    log1("Window %d closed\n", e->windowID);
		    break;
	};
}

void run0() {
	SDL_RenderClear( renderer);
	SDLGUI_Draw();
	SDL_RenderPresent( renderer);

	SDL_Event e;
	while( createMapDialogData.running) {
		SDL_WaitEvent( &e);
		switch (e.type) {
			case SDL_WINDOWEVENT: {
				SDL_WindowEvent *e2 = &e.window;
				if( e2->event == SDL_WINDOWEVENT_RESIZED)
					resizeView( GUI_LEFTPANEL_WIDTH, e2->data1, e2->data2);
				break;
			}
			case SDL_QUIT:
				createMapDialogData.running = false;
				buttonQuit_clicked( NULL);
				break;
			case SDL_KEYDOWN:
				if( isMessageBoxOn) {
					SDLGUI_Hide_Message();
					isMessageBoxOn =false;
				}
				else if ( SDLGUI_Handle_TextInput( (SDL_KeyboardEvent*)&e) != true) {
					if( ((SDL_KeyboardEvent*)&e)->keysym.sym == SDLK_q) {
						createMapDialogData.running = false;
						buttonQuit_clicked( NULL);
					}
				}
				continue;
			case SDL_MOUSEBUTTONDOWN:
				if( isMessageBoxOn) {
					SDLGUI_Hide_Message();
					isMessageBoxOn =false;
					continue;
				}
				if( SDLGUI_Handle_MouseDown( (SDL_MouseButtonEvent*)&e)) {
					mouseDownInGui = true;
				}
				continue;
			case SDL_MOUSEBUTTONUP:
				if( isMessageBoxOn) {
					SDLGUI_Hide_Message();
					isMessageBoxOn =false;
				}
				else if( mouseDownInGui) {
					SDLGUI_Handle_MouseUp( (SDL_MouseButtonEvent*)&e);
				}
				break;
		};
		SDL_SetRenderDrawColor( renderer, 0,0,0,255);
		SDL_RenderClear( renderer);
		SDLGUI_Draw();
		SDL_RenderPresent( renderer);
	}
}

void run() {
	drawBackground();
	draw();
	SDL_Event e;
	while( running) {
		SDL_WaitEvent( &e);
		switch (e.type) {
			case SDL_WINDOWEVENT:
				handleWindowEvent( &e.window);
				break;
			case SDL_QUIT:
				quit("Quitting");
				running =0;
				break;
			case SDL_KEYDOWN:
				if( isMessageBoxOn) {
					log2("msg box on\n");
					SDLGUI_Hide_Message();
					isMessageBoxOn =false;
					break;
				}
				else if(SDLGUI_Handle_TextInput( (SDL_KeyboardEvent*)&e) != true)
					handleKey( (SDL_KeyboardEvent*)&e);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if( isMessageBoxOn) {
					SDLGUI_Hide_Message();
					isMessageBoxOn =false;
					break;
				}
				if( SDLGUI_Handle_MouseDown( (SDL_MouseButtonEvent*)&e)) {
					log2("mouse down - gui\n");
					mouseDownInGui = true;
				}
				else {
					log2("mouse down - editor\n");
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
				}
				else if( mouseDownInGui) {
					SDLGUI_Handle_MouseUp( (SDL_MouseButtonEvent*)&e);
				}
				break;
			case SDL_MOUSEMOTION:
				if( mouseDownInGui != true && handleMouse( 0, (SDL_MouseMotionEvent*)&e) ) {
					drawBackground();
					break;
				}
				else
					continue;
			case SDL_KEYUP:
				/*don't do anything for those events*/
				continue;
			case SDL_USEREVENT: {
				void (*p) (void*) = e.user.data1;
				p( e.user.data2);
				break;
			}
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
				drawTexture( renderer, 
					textures->obj[obj->type]->textures[ obj->visualState][obj->dir], 
					screenPos.i*TILELEN + GUI_LEFTPANEL_WIDTH, screenPos.j*TILELEN, TILELEN, TILELEN );
				if( obj == selectedObj) {
					drawTexture( renderer, textures->highlitObjIndicator, screenPos.i*TILELEN + GUI_LEFTPANEL_WIDTH, screenPos.j*TILELEN, TILELEN, TILELEN);
				}
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
	
	drawTexture( renderer, bgroundTexture, GUI_LEFTPANEL_WIDTH, 0, viewSize.i*TILELEN, viewSize.j*TILELEN);
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
			CREATE_LIST_BUTTON( 0, "1. terrain", CREATE_BRUSH_WRAPPER( SDLK_1, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "1. ground", 			CREATE_BRUSH_WRAPPER( SDLK_1, &drawTerrain, terrain_gnd, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "2. wall", 	 			CREATE_BRUSH_WRAPPER( SDLK_2, &drawTerrain, terrain_wall, 	NO_CHILDREN)),
				}, 2
			))),
			CREATE_LIST_BUTTON( 1, "2. object", CREATE_BRUSH_WRAPPER( SDLK_2, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "1. select", 	CREATE_BRUSH_WRAPPER( SDLK_1, &editor_selectObj, 	NO_VAR, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "2. create", 	CREATE_BRUSH_WRAPPER( SDLK_2, &editor_createObj,	go_apple, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 2, "3. change type",CREATE_BRUSH_WRAPPER( SDLK_3, NO_FUN, 				NO_VAR, 	/*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
							CREATE_LIST_BUTTON( 0, "1. player", 			CREATE_BRUSH_WRAPPER( SDLK_1, &editor_changeObjType, go_player, 		NO_CHILDREN)),
							CREATE_LIST_BUTTON( 1, "2. left turner", 		CREATE_BRUSH_WRAPPER( SDLK_2, &editor_changeObjType, go_leftTurner, 	NO_CHILDREN)),
							CREATE_LIST_BUTTON( 2, "3. apple", 				CREATE_BRUSH_WRAPPER( SDLK_3, &editor_changeObjType, go_apple, 			NO_CHILDREN)),
							CREATE_LIST_BUTTON( 3, "4. flower", 			CREATE_BRUSH_WRAPPER( SDLK_4, &editor_changeObjType, go_flower, 		NO_CHILDREN)),
							CREATE_LIST_BUTTON( 4, "5. creeper plant", 		CREATE_BRUSH_WRAPPER( SDLK_5, &editor_changeObjType, go_creeperPlant, 	NO_CHILDREN)),
							CREATE_LIST_BUTTON( 5, "6. peekaboo monster", 	CREATE_BRUSH_WRAPPER( SDLK_6, &editor_changeObjType, go_peekaboo, 		NO_CHILDREN)),
							CREATE_LIST_BUTTON( 6, "7. weeping angel", 		CREATE_BRUSH_WRAPPER( SDLK_7, &editor_changeObjType, ai_weepingAngel, 	NO_CHILDREN)),
						}, 7
					))),
					CREATE_LIST_BUTTON( 3, "4. remove", CREATE_BRUSH_WRAPPER( SDLK_4, &eraseObject, NO_VAR, NO_CHILDREN)),
					CREATE_LIST_BUTTON( 4, "5. rotate", CREATE_BRUSH_WRAPPER( SDLK_5, NO_FUN, NO_VAR, 		/*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
							CREATE_LIST_BUTTON( 0, "(1) \x80 up", 		CREATE_BRUSH_WRAPPER( SDLK_1, &setDirection, dir_up, 		NO_CHILDREN)),
							CREATE_LIST_BUTTON( 1, "(2) \x81 right", 	CREATE_BRUSH_WRAPPER( SDLK_2, &setDirection, dir_right, 	NO_CHILDREN)),
							CREATE_LIST_BUTTON( 2, "(3) \x82 down", 	CREATE_BRUSH_WRAPPER( SDLK_3, &setDirection, dir_down,		NO_CHILDREN)),
							CREATE_LIST_BUTTON( 3, "(4) \x83 left", 	CREATE_BRUSH_WRAPPER( SDLK_4, &setDirection, dir_left,		NO_CHILDREN)),
						}, 4
					))),
				}, 5
			))),
			CREATE_LIST_BUTTON( 2, "3. ai", CREATE_BRUSH_WRAPPER(/*key*/SDLK_3, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "1. erase",	CREATE_BRUSH_WRAPPER( SDLK_1, &eraseAI, NO_VAR, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "2. put", 	CREATE_BRUSH_WRAPPER( SDLK_2, NO_FUN, 	NO_VAR, 	/*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
							CREATE_LIST_BUTTON( 0, "(1) left turner", 		CREATE_BRUSH_WRAPPER( SDLK_1, &drawAI, ai_leftTurner, 		NO_CHILDREN)),
							CREATE_LIST_BUTTON( 1, "(2) hungry left turner",CREATE_BRUSH_WRAPPER( SDLK_2, &drawAI, ai_hungryLeftTurner, NO_CHILDREN)),
							CREATE_LIST_BUTTON( 2, "(3) simple flower", 	CREATE_BRUSH_WRAPPER( SDLK_3, &drawAI, ai_simpleFlower, 	NO_CHILDREN)),
							CREATE_LIST_BUTTON( 3, "(4) creeper plant", 	CREATE_BRUSH_WRAPPER( SDLK_4, &drawAI, ai_creeperPlant, 	NO_CHILDREN)),
							CREATE_LIST_BUTTON( 4, "(5) peek-a-boo chaser", CREATE_BRUSH_WRAPPER( SDLK_5, &drawAI, ai_peekaboo, 		NO_CHILDREN)),
							CREATE_LIST_BUTTON( 5, "(6) weeping angel", 	CREATE_BRUSH_WRAPPER( SDLK_6, &drawAI, ai_weepingAngel, 	NO_CHILDREN)),
						}, 6
					))),
				}, 2
			))),
		}, 3
	);
	SDLGUI_Set_Panel_Elements( brushContainer, brushList, true);

	SDLGUI_List_Add( bodyItems, SDLGUI_Create_Text( 10, 725, -1, 25, NULL, "Selected Obj:", 	(int[4]){0,0,0,0}, 			(int[4]){0,0,0,255}, 12, 16, 0, NULL));
	struct SDLGUI_Element *selectedObjContainer = SDLGUI_Create_Panel( 10, 750, GUI_LEFTPANEL_WIDTH - 2*10, 100, (int[4]){0,0,0,0}, (int[4]){0,0,0,255}, 1);
	SDLGUI_List_Add( bodyItems, selectedObjContainer);
	struct SDLGUI_List *selectedObjElements = SDLGUI_Get_Panel_Elements( selectedObjContainer);

	textbox_health = 	SDLGUI_Create_Textbox( 	120, 10, 5, (int[4]){255,255,255,255},  (int[4]){0,0,0,255}, 6, 8, &textbox_health_changed);
	textbox_maxHealth = SDLGUI_Create_Textbox( 	120, 30, 5, (int[4]){255,255,255,255},  (int[4]){0,0,0,255}, 6, 8, &textbox_maxHealth_changed);

	SDLGUI_List_Add( selectedObjElements, SDLGUI_Create_Text( 30,  10, -1, 16, NULL, "    Health :", 	(int[4]){0,0,0,0}, 			(int[4]){0,0,0,255}, 6, 8, 0, NULL));
	SDLGUI_List_Add( selectedObjElements, SDLGUI_Create_Text( 30,  30, -1, 16, NULL, "Max-Health :", 	(int[4]){0,0,0,0}, 			(int[4]){0,0,0,255}, 6, 8, 0, NULL));
	SDLGUI_List_Add( selectedObjElements, textbox_maxHealth);
	SDLGUI_List_Add( selectedObjElements, textbox_health);
}
#undef CREATE_LIST_BUTTON
#undef NO_VAR
#undef NO_CHILDREN
#undef NO_FUN

void editor_createMap_clicked( struct SDLGUI_Element *from) {
	int width = parseText( SDLGUI_GetText_Textbox( createMapDialogData.textbox_width  ) );
	int height = parseText( SDLGUI_GetText_Textbox( createMapDialogData.textbox_height) );
	if( width <= 0 || height <= 0) {
		isMessageBoxOn = true;
		SDLGUI_Show_Message(0, 0, windowW, windowH, SDLGUI_MESSAGE_ERROR, "Width & height must be >= 0");
	}
	else {
		createMapDialogData.running = false;
		//TODO I could just dynamically allocate createMapDialogData, then deallocate it here aswell

		myMap = createNewMap( width, height);
		myMap->filePath = "test-map.yz01.map";
	
		running = true;
	}
}

int main( int argc, char *args[]) {
	//Default values
	myMap = 0;

	init();
    log0("loading textures\n");
	textures = loadAllTextures( renderer);
	SDLGUI_Init( renderer, textures->font);
	
	running = false;
	createMapDialogData.running = true;

	if( argc > 1) {
		myMap = readMapFile( args[1]);
		myMap->filePath = args[1];
		running = true;
	}
	else {
		createMapDialogData.panel = SDLGUI_Create_Panel( 0, 0, windowW, windowH, (int[4]){0,0,0,255}, (int[4]){0,0,0,0}, 0);
		SDLGUI_Add_Element( createMapDialogData.panel);

		struct SDLGUI_List *panelElems = SDLGUI_Get_Panel_Elements( createMapDialogData.panel);
		SDLGUI_List_Add( panelElems, SDLGUI_Create_Text( 0, 100, windowW, 100, NULL, "Creating New Map\n----------------", (int[4]){0,0,0,0}, (int[4]){255,255,255,255}, 12, 16, 0, NULL));
		SDLGUI_List_Add( panelElems, SDLGUI_Create_Text( windowW/2 - 100, 200, -1, 32, NULL, " width", (int[4]){0,0,0,0}, (int[4]){255,255,255,255}, 12, 16, 0, NULL));
		SDLGUI_List_Add( panelElems, SDLGUI_Create_Text( windowW/2 - 100, 230, -1, 32, NULL, "height", (int[4]){0,0,0,0}, (int[4]){255,255,255,255}, 12, 16, 0, NULL));
		SDLGUI_List_Add( panelElems, SDLGUI_Create_Text( windowW/2 - 100, 300, 200, 100, &editor_createMap_clicked, "create", (int[4]){0,0,0,0}, (int[4]){255,255,255,255}, 12, 16, 2, NULL));

		createMapDialogData.textbox_width  = SDLGUI_Create_Textbox( windowW/2, 200, 2, (int[4]){255,255,255,50}, (int[4]){255,255,255,255}, 12, 16, NULL);
		createMapDialogData.textbox_height = SDLGUI_Create_Textbox( windowW/2, 230, 2, (int[4]){255,255,255,50}, (int[4]){255,255,255,255}, 12, 16, NULL);
		SDLGUI_List_Add( panelElems, createMapDialogData.textbox_width );
		SDLGUI_List_Add( panelElems, createMapDialogData.textbox_height);
		
		run0();
	}

	if( running) {
		SDLGUI_Destroy();
		SDLGUI_Init( renderer, textures->font);
		initGui();
		run();
	}

	log0("Program over\nPeace\n");
	SDLGUI_Destroy();
	SDL_DestroyRenderer( renderer);
	SDL_DestroyWindow( window);
	SDL_Quit();
	return 0;
}
