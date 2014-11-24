#include "engine.h"
#include "brush.h"
#include "aiTable.h"

#include "sdl2gui/sdl2gui.h"
#include "sdl2gui/sdl2gui-list.h"
#include "stack.h"

#include "error.h"

bool running;
unsigned int objectCounter = 1;

bool moveForward( struct Map *map, struct object* obj) { return false; }
bool turnLeft( struct Map *map, struct object *obj) { return false; }
bool turnRight( struct Map *map, struct object* obj) { return false; }

/* GUI Elements */
#define GUI_LEFTPANEL_WIDTH 192
#define GUI_TOPBAR_HEIGHT 32
#define GUI_UGLY_BGCOLOR (int[4]){170,180,190,255}
#define GUI_UGLY_BORDERCOLOR (int[4]){100,100,100,255}
struct SDLGUI_Element *bodyContainer;
struct SDLGUI_Element *topbar;
struct SDLGUI_Element *brushContainer;
struct SDLGUI_Element *selectedObjContainer;
struct SDLGUI_List *brushList;
bool mouseDownInGui;
bool isMessageBoxOn = false;

struct SDLGUI_List *brushStack[5];
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

struct SDLGUI_Element *textbox_id;
struct SDLGUI_Element *textbox_health;
struct SDLGUI_Element *textbox_maxHealth;
struct SDLGUI_Element *textbox_healthGiven;
struct SDLGUI_Element *textbox_movable;
struct SDLGUI_Element *textbox_pickable;
struct SDLGUI_Element *textbox_attack;
struct SDLGUI_Element *textbox_defence;
struct SDLGUI_Element *textbox_pos_x, *textbox_pos_y;
struct object *selectedObj = NULL;

struct {
	struct SDLGUI_Element *panel, *textbox_width, *textbox_height, *textbox_name;
	bool running;
} createMapDialogData;

#define SHOW_TOOLTIP( x, y, text) SDLGUI_Show_Tooltip( x*TILELEN + GUI_LEFTPANEL_WIDTH, y*TILELEN + GUI_TOPBAR_HEIGHT, text)


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
		
		SHOW_TOOLTIP( x, y, "AI removed");

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

bool editor_changeObjType( unsigned int x, unsigned int y, int type) {
	if( myMap->objs[x][y]==NULL)
		return false;
	
	myMap->objs[x][y]->type = type;

	return true;
}

bool editor_selectObj( unsigned int x, unsigned int y, int type) {
	
	selectedObj = myMap->objs[x][y];
	
	char tmp[4];
	if( selectedObj != NULL) {
		selectedObjContainer->isVisible = true;

    	sprintf(tmp, "%d", selectedObj->id);
		SDLGUI_SetText_Textbox( textbox_id, tmp);

		sprintf(tmp, "%d", selectedObj->health);
		SDLGUI_SetText_Textbox( textbox_health, tmp);

		sprintf(tmp, "%d", selectedObj->maxHealth);
		SDLGUI_SetText_Textbox( textbox_maxHealth, tmp);

		sprintf(tmp, "%d", selectedObj->healthGiven);
		SDLGUI_SetText_Textbox( textbox_healthGiven, tmp);

		sprintf(tmp, "%d", selectedObj->isMovable);
		SDLGUI_SetText_Textbox( textbox_movable, tmp);

		sprintf(tmp, "%d", selectedObj->isPickable);
		SDLGUI_SetText_Textbox( textbox_pickable, tmp);

		sprintf(tmp, "%d", selectedObj->attack);
		SDLGUI_SetText_Textbox( textbox_attack, tmp);

		sprintf(tmp, "%d", selectedObj->defence);
		SDLGUI_SetText_Textbox( textbox_defence, tmp);
	}
	else {
		selectedObjContainer->isVisible = false;
	}
	
	sprintf( tmp, "%d", x);
	SDLGUI_SetText_Textbox( textbox_pos_x, tmp);
	
	sprintf( tmp, "%d", y);
	SDLGUI_SetText_Textbox( textbox_pos_y, tmp);

	return true;
}

bool editor_createObj( unsigned int x, unsigned int y, int type){
	//add objects only if there is no object there
	if( myMap->objs[x][y]!=NULL || myMap->tiles[x][y]!=terrain_gnd)
		return false;

	//create and initialize a monster
	struct object *obj = createObject( type, x, y, objectCounter++);


	addObject( obj, myMap, x, y);
	editor_selectObj( x, y, type);
	
	char tooltipText[8];
	sprintf( tooltipText, "#%d", obj->id);
	SHOW_TOOLTIP( x, y, tooltipText);

	return true;
}

bool drawAI( unsigned int x, unsigned int y, int type) {
	//if there is an object at the given location, and it dsoen't have an AI
	if( myMap->objs[x][y] != 0) {
		if ( myMap->objs[x][y]->ai != NULL && myMap->objs[x][y]->ai->type != type) 
			AI_DESTROY( myMap->objs[x][y]->ai);

		struct AI *ai = AI_CREATE( type);
		myMap->objs[x][y]->ai = ai;

		SHOW_TOOLTIP( x, y, "AI put");
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

void textbox_id_changed( struct SDLGUI_Element *textbox, const char *text) {
	if( selectedObj && strlen(text) > 0) {
		int newId = atoi( text);
		if( newId >= 1) {
			selectedObj->id = newId;
		} 
		else {
			SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_ERROR, "You must give id >= 1");
			isMessageBoxOn = true;
		}
	}
}

void textbox_health_changed( struct SDLGUI_Element *textbox, const char *text) {
	if( selectedObj) {
		int newHealth = parseText( text);
		if( newHealth <= UINT8_MAX) {
			selectedObj->health = newHealth;
		}
		else {
			SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_ERROR, "Health should be <= 256");
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
		if( newHealth <= UINT8_MAX) {
			selectedObj->maxHealth = newHealth;
		}
		else {
			SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_ERROR, "Max-Health should be <= 256");
			isMessageBoxOn = true;
		}
	}
	else {
		SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_WARNING, "No object selected");
		isMessageBoxOn = true;
	}
}

void textbox_healthGiven_changed( struct SDLGUI_Element *textbox, const char *text) {
	if( selectedObj) {
		int newValue = parseText( text);
		if( newValue <= INT8_MAX && newValue >= INT8_MIN) {
			selectedObj->healthGiven = newValue;
		}
		else {
			SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_ERROR, "Health-Given must be >= -128 and <= 127");
			isMessageBoxOn = true;
		}
	}
	else {
		SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_WARNING, "No object selected");
		isMessageBoxOn = true;
	}
}

void textbox_movable_changed( struct SDLGUI_Element *textbox, const char *text) {
	if( selectedObj) {
		int newValue = parseText( text);
		if( newValue == 1 || newValue == 0) {
			selectedObj->isMovable = newValue;
		}
		else {
			SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_ERROR, "Movable value can be 1 or 0");
			isMessageBoxOn = true;
		}
	}
	else {
		SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_WARNING, "No object selected");
		isMessageBoxOn = true;
	}
}

void textbox_pickable_changed( struct SDLGUI_Element *textbox, const char *text) {
	if( selectedObj) {
		int newValue = parseText( text);
		if( newValue == 1 || newValue == 0) {
			selectedObj->isPickable = newValue;
		}
		else {
			SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_ERROR, "Pickable value can be 1 or 0");
			isMessageBoxOn = true;
		}
	}
	else {
		SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_WARNING, "No object selected");
		isMessageBoxOn = true;
	}
}

void textbox_attack_changed( struct SDLGUI_Element *textbox, const char *text) {
	if( selectedObj) {
		int newValue = parseText( text);
		if( newValue <= UINT8_MAX ) {
			selectedObj->attack = newValue;
		}
		else {
			SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_ERROR, "Attack can be <= 256");
			isMessageBoxOn = true;
		}
	}
	else {
		SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_WARNING, "No object selected");
		isMessageBoxOn = true;
	}
}

void textbox_defence_changed( struct SDLGUI_Element *textbox, const char *text) {
	if( selectedObj) {
		int newValue = parseText( text);
		if( newValue <= UINT8_MAX ) {
			selectedObj->defence = newValue;
		}
		else {
			SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_ERROR, "Defence can be <= 256");
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
		y = (e->y -  GUI_TOPBAR_HEIGHT) / TILELEN;
	}
	else {
		if( ! e2->state) //if no buttons are pressed
			return 0;
		x = (e2->x - GUI_LEFTPANEL_WIDTH) / TILELEN;
		y = (e2->y - GUI_TOPBAR_HEIGHT)/ TILELEN;
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
			resizeView( GUI_LEFTPANEL_WIDTH, GUI_TOPBAR_HEIGHT, e->data1, e->data2);
			bodyContainer->rect.h = e->data2;
			topbar->rect.w = e->data1 - GUI_LEFTPANEL_WIDTH;
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
					resizeView( GUI_LEFTPANEL_WIDTH, GUI_TOPBAR_HEIGHT, e2->data1, e2->data2);
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
					screenPos.i*TILELEN + GUI_LEFTPANEL_WIDTH, screenPos.j*TILELEN + GUI_TOPBAR_HEIGHT, TILELEN, TILELEN );
				if( obj == selectedObj) {
					drawTexture( renderer, textures->highlitObjIndicator, screenPos.i*TILELEN + GUI_LEFTPANEL_WIDTH, screenPos.j*TILELEN + GUI_TOPBAR_HEIGHT, TILELEN, TILELEN);
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
	
	drawTexture( renderer, bgroundTexture, GUI_LEFTPANEL_WIDTH, GUI_TOPBAR_HEIGHT, viewSize.i*TILELEN, viewSize.j*TILELEN);
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
#define NO_CLICK_HANDLER NULL
#define CREATE_LIST_BUTTON( i, text, data) SDLGUI_Create_Text( 5, 5 + 35*i, 160, 30, &brushListItem_clicked, text, (int[4]){255,255,255,255}, (int[4]){0,0,0,255}, 6, 9, 1, data)

#define TEMPLATES_PATH "templates.csv"
#define DELIMETER ","
#define MAX_NAME_LENGTH 10
#define MAX_TEMPLATES_COUNT 8

#define CHECK_STRTOK_RESULT( string, no, name) EXIT_IF( string == NULL, "Error at line %d. Line ended but was expeting %s\n", no, name)

struct object *objectTemplates[MAX_TEMPLATES_COUNT];

bool applyObjTemplate( unsigned int x, unsigned int y, int templateNo) {
	struct object *to = myMap->objs[x][y];
	struct object *from = objectTemplates[ templateNo];

	if( from != NULL && to != NULL) {
		if( from->id != 0)
			to->id = 0;
		to->type = from->type;
		to->health = from->health;
		to->maxHealth = from->maxHealth;
		to->healthGiven = from->healthGiven;
		to->isMovable = from->isMovable;
		to->isPickable = from->isPickable;
		to->attack = from->attack;
		to->defence = from->defence;

		return true;
	}
	else
		return false;
}

struct SDLGUI_Element* parseTemplateLine( char *line, int lineNo) {
	char *string;
	const char *delim = DELIMETER;

	char *name;

	struct object *template = (struct object*)malloc(sizeof(struct object));;

	template->id = 0;

	string = strtok( line, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "name");
	EXIT_IF( strlen( string) > MAX_NAME_LENGTH, "template name %s is too long(%d) in line %d. Must be < %d\n", string, (int)strlen( string), lineNo, MAX_NAME_LENGTH);
	name = string;

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "obj-type");
	template->type = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "ai-type");
	//template->aiType = atoi( string);
	
	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "health");
	template->health = atoi( string);
	
	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "maxHealth");
	template->maxHealth = atoi( string);
	
	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "healthGiven");
	template->healthGiven = atoi( string);
	
	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "isMovable");
	template->isMovable = atoi( string);
	
	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "isPickable");
	template->isPickable = atoi( string);
	
	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "attack");
	template->attack = atoi( string);
	
	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "defence");
	template->defence = atoi( string);

	char buttonText[32];
	sprintf( buttonText, "%d. %s", lineNo+1, name);

	objectTemplates[ lineNo] = template;
	return CREATE_LIST_BUTTON( lineNo, buttonText, CREATE_BRUSH_WRAPPER( SDLK_1 + lineNo, applyObjTemplate, lineNo, NO_CHILDREN));
}


struct SDLGUI_List* reloadBrushTemplates() {
	struct SDLGUI_List *templateBrushes = SDLGUI_List_Create( MAX_TEMPLATES_COUNT/2);;

	int i;
	for( i=0; i<MAX_TEMPLATES_COUNT; i++)
		objectTemplates[i] = NULL;

	FILE *fp = fopen( TEMPLATES_PATH, "r");
	if( fp == NULL)
		return templateBrushes;

	int lineNo = 0;
	char line[BUFSIZ];

	while( fgets( line, BUFSIZ, fp) ) {
		if( lineNo > MAX_TEMPLATES_COUNT) {
			fprintf( stderr, "Max %d templates supported.\n", MAX_TEMPLATES_COUNT);
			break;
		}

		SDLGUI_List_Add( templateBrushes, parseTemplateLine( line, lineNo));
		
		lineNo ++;
	}

	fclose( fp);

	return templateBrushes;
}
void initGui() {
    /* The left panel */
	bodyContainer = SDLGUI_Create_Panel( 0, 0, GUI_LEFTPANEL_WIDTH, 960, GUI_UGLY_BGCOLOR, GUI_UGLY_BORDERCOLOR, 4);
	
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
			CREATE_LIST_BUTTON( 0, "1. select", CREATE_BRUSH_WRAPPER( SDLK_1, &editor_selectObj, 	NO_VAR, 	NO_CHILDREN)),
			CREATE_LIST_BUTTON( 1, "2. terrain", CREATE_BRUSH_WRAPPER( SDLK_2, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "1. ground", 			CREATE_BRUSH_WRAPPER( SDLK_1, &drawTerrain, terrain_gnd, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "2. wall", 	 			CREATE_BRUSH_WRAPPER( SDLK_2, &drawTerrain, terrain_wall, 	NO_CHILDREN)),
				}, 2
			))),
			CREATE_LIST_BUTTON( 2, "3. object", CREATE_BRUSH_WRAPPER( SDLK_3, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "1. create", 	CREATE_BRUSH_WRAPPER( SDLK_1, &editor_createObj,	go_apple, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "2. change type",CREATE_BRUSH_WRAPPER( SDLK_2, NO_FUN, 				NO_VAR, 	/*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
							CREATE_LIST_BUTTON( 0, "1. player", 			CREATE_BRUSH_WRAPPER( SDLK_1, &editor_changeObjType, go_player, 		NO_CHILDREN)),
							CREATE_LIST_BUTTON( 1, "2. inanimate", 			CREATE_BRUSH_WRAPPER( SDLK_2, &editor_changeObjType, go_apple, 			SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){
									CREATE_LIST_BUTTON( 0, "1. apple", 			CREATE_BRUSH_WRAPPER( SDLK_1, &editor_changeObjType, go_apple, 			NO_CHILDREN)),
									CREATE_LIST_BUTTON( 1, "2. wall/door",		CREATE_BRUSH_WRAPPER( SDLK_2, &editor_changeObjType, go_door,			NO_CHILDREN)),
									CREATE_LIST_BUTTON( 2, "3. button",			CREATE_BRUSH_WRAPPER( SDLK_3, &editor_changeObjType, go_button,			NO_CHILDREN)),
									CREATE_LIST_BUTTON( 3, "4. switch",			CREATE_BRUSH_WRAPPER( SDLK_4, &editor_changeObjType, go_switch,			NO_CHILDREN)),
									CREATE_LIST_BUTTON( 4, "5. sensors",		CREATE_BRUSH_WRAPPER( SDLK_5, &editor_changeObjType, go_lineSensor,		SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){
											CREATE_LIST_BUTTON( 0, "1. line",		CREATE_BRUSH_WRAPPER( SDLK_1, &editor_changeObjType, go_lineSensor,		NO_CHILDREN)),
										}, 1
									))),
								}, 5
							))),
							CREATE_LIST_BUTTON( 2, "3. animate", 			CREATE_BRUSH_WRAPPER( SDLK_3, &editor_changeObjType, go_leftTurner, 	SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){
									CREATE_LIST_BUTTON( 0, "1. left turner", 		CREATE_BRUSH_WRAPPER( SDLK_1, &editor_changeObjType, go_leftTurner, 	NO_CHILDREN)),
									CREATE_LIST_BUTTON( 1, "2. flower", 			CREATE_BRUSH_WRAPPER( SDLK_2, &editor_changeObjType, go_flower, 		NO_CHILDREN)),
									CREATE_LIST_BUTTON( 2, "3. creeper plant", 		CREATE_BRUSH_WRAPPER( SDLK_3, &editor_changeObjType, go_creeperPlant, 	NO_CHILDREN)),
									CREATE_LIST_BUTTON( 3, "4. peekaboo monster", 	CREATE_BRUSH_WRAPPER( SDLK_4, &editor_changeObjType, go_peekaboo, 		NO_CHILDREN)),
									CREATE_LIST_BUTTON( 4, "5. weeping angel", 		CREATE_BRUSH_WRAPPER( SDLK_5, &editor_changeObjType, go_weepingAngel, 	NO_CHILDREN)),
								}, 5
							))),
						}, 3
					))),
					CREATE_LIST_BUTTON( 2, "3. remove", CREATE_BRUSH_WRAPPER( SDLK_3, &eraseObject, NO_VAR, NO_CHILDREN)),
					CREATE_LIST_BUTTON( 3, "4. rotate", CREATE_BRUSH_WRAPPER( SDLK_4, NO_FUN, NO_VAR, 		/*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
							CREATE_LIST_BUTTON( 0, "(1) \x80 up", 		CREATE_BRUSH_WRAPPER( SDLK_1, &setDirection, dir_up, 		NO_CHILDREN)),
							CREATE_LIST_BUTTON( 1, "(2) \x81 right", 	CREATE_BRUSH_WRAPPER( SDLK_2, &setDirection, dir_right, 	NO_CHILDREN)),
							CREATE_LIST_BUTTON( 2, "(3) \x82 down", 	CREATE_BRUSH_WRAPPER( SDLK_3, &setDirection, dir_down,		NO_CHILDREN)),
							CREATE_LIST_BUTTON( 3, "(4) \x83 left", 	CREATE_BRUSH_WRAPPER( SDLK_4, &setDirection, dir_left,		NO_CHILDREN)),
						}, 4
					))),
					CREATE_LIST_BUTTON( 4, "5. templates", CREATE_BRUSH_WRAPPER( SDLK_5, NO_FUN, NO_VAR, reloadBrushTemplates())),
				}, 5
			))),
			CREATE_LIST_BUTTON( 3, "4. ai", CREATE_BRUSH_WRAPPER(/*key*/SDLK_4, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "1. erase",	CREATE_BRUSH_WRAPPER( SDLK_1, &eraseAI, NO_VAR, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "2. put", 	CREATE_BRUSH_WRAPPER( SDLK_2, NO_FUN, 	NO_VAR, 	/*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
							CREATE_LIST_BUTTON( 0, "(1) left turner", 		CREATE_BRUSH_WRAPPER( SDLK_1, &drawAI, ai_leftTurner, 		NO_CHILDREN)),
							CREATE_LIST_BUTTON( 1, "(2) hungry left turner",CREATE_BRUSH_WRAPPER( SDLK_2, &drawAI, ai_hungryLeftTurner, NO_CHILDREN)),
							CREATE_LIST_BUTTON( 2, "(3) simple flower", 	CREATE_BRUSH_WRAPPER( SDLK_3, &drawAI, ai_simpleFlower, 	NO_CHILDREN)),
							CREATE_LIST_BUTTON( 3, "(4) creeper plant", 	CREATE_BRUSH_WRAPPER( SDLK_4, &drawAI, ai_creeperPlant, 	NO_CHILDREN)),
							CREATE_LIST_BUTTON( 4, "(5) peek-a-boo chaser", CREATE_BRUSH_WRAPPER( SDLK_5, &drawAI, ai_peekaboo, 		NO_CHILDREN)),
							CREATE_LIST_BUTTON( 5, "(6) weeping angel", 	CREATE_BRUSH_WRAPPER( SDLK_6, &drawAI, ai_weepingAngel, 	NO_CHILDREN)),
							CREATE_LIST_BUTTON( 6, "(7) slide door", 		CREATE_BRUSH_WRAPPER( SDLK_7, &drawAI, ai_door, 			NO_CHILDREN)),
							CREATE_LIST_BUTTON( 7, "(8) line sensor", 		CREATE_BRUSH_WRAPPER( SDLK_8, &drawAI, ai_lineSensor,		NO_CHILDREN)),
						}, 8
					))),
				}, 2
			))),
		}, 4
	);
	SDLGUI_Set_Panel_Elements( brushContainer, brushList, true);

    /* Selected Object Panel
    */
	selectedObjContainer = SDLGUI_Create_Panel( 10, 720, GUI_LEFTPANEL_WIDTH - 2*10, 230, (int[4]){0,0,0,0}, (int[4]){0,0,0,255}, 1);
    selectedObjContainer->isVisible = false;

    textbox_id =        	SDLGUI_Create_Textbox( 	120,  40,  5, TEXTBOX_INPUT_NUMERIC, (int[4]){255,255,255,255},  (int[4]){0,0,0,255}, 6, 8, &textbox_id_changed);
	textbox_health = 		SDLGUI_Create_Textbox( 	120,  60,  5, TEXTBOX_INPUT_NUMERIC, (int[4]){255,255,255,255},  (int[4]){0,0,0,255}, 6, 8, &textbox_health_changed);
	textbox_maxHealth = 	SDLGUI_Create_Textbox( 	120,  80,  5, TEXTBOX_INPUT_NUMERIC, (int[4]){255,255,255,255},  (int[4]){0,0,0,255}, 6, 8, &textbox_maxHealth_changed);
	textbox_healthGiven = 	SDLGUI_Create_Textbox( 	120, 100,  5, TEXTBOX_INPUT_NUMERIC, (int[4]){255,255,255,255},  (int[4]){0,0,0,255}, 6, 8, &textbox_healthGiven_changed);
	textbox_movable = 		SDLGUI_Create_Textbox( 	120, 120,  5, TEXTBOX_INPUT_NUMERIC, (int[4]){255,255,255,255},  (int[4]){0,0,0,255}, 6, 8, &textbox_movable_changed);
	textbox_pickable = 		SDLGUI_Create_Textbox( 	120, 140,  5, TEXTBOX_INPUT_NUMERIC, (int[4]){255,255,255,255},  (int[4]){0,0,0,255}, 6, 8, &textbox_pickable_changed);
	textbox_attack = 		SDLGUI_Create_Textbox( 	120, 160,  5, TEXTBOX_INPUT_NUMERIC, (int[4]){255,255,255,255},  (int[4]){0,0,0,255}, 6, 8, &textbox_attack_changed);
	textbox_defence = 		SDLGUI_Create_Textbox( 	120, 180,  5, TEXTBOX_INPUT_NUMERIC, (int[4]){255,255,255,255},  (int[4]){0,0,0,255}, 6, 8, &textbox_defence_changed);

	struct SDLGUI_List *selectedObjElements = SDLGUI_Get_Panel_Elements( selectedObjContainer);
	SDLGUI_List_Add( selectedObjElements, SDLGUI_Create_Text( 10,  10, -1, 25, NULL, "Selected Obj:", 	(int[4]){0,0,0,0}, 			(int[4]){0,0,0,255}, 12, 16, 0, NULL));
	SDLGUI_List_Add( selectedObjElements, SDLGUI_Create_Text( 30,  40, -1, 16, NULL, "          ID :", 	(int[4]){0,0,0,0}, 			(int[4]){0,0,0,255}, 6, 8, 0, NULL));
	SDLGUI_List_Add( selectedObjElements, SDLGUI_Create_Text( 30,  60, -1, 16, NULL, "      Health :", 	(int[4]){0,0,0,0}, 			(int[4]){0,0,0,255}, 6, 8, 0, NULL));
	SDLGUI_List_Add( selectedObjElements, SDLGUI_Create_Text( 30,  80, -1, 16, NULL, "  Max-Health :", 	(int[4]){0,0,0,0}, 			(int[4]){0,0,0,255}, 6, 8, 0, NULL));
	SDLGUI_List_Add( selectedObjElements, SDLGUI_Create_Text( 30, 100, -1, 16, NULL, "Health Given :", 	(int[4]){0,0,0,0}, 			(int[4]){0,0,0,255}, 6, 8, 0, NULL));
	SDLGUI_List_Add( selectedObjElements, SDLGUI_Create_Text( 30, 120, -1, 16, NULL, "     Movable :", 	(int[4]){0,0,0,0}, 			(int[4]){0,0,0,255}, 6, 8, 0, NULL));
	SDLGUI_List_Add( selectedObjElements, SDLGUI_Create_Text( 30, 140, -1, 16, NULL, "    Pickable :", 	(int[4]){0,0,0,0}, 			(int[4]){0,0,0,255}, 6, 8, 0, NULL));
	SDLGUI_List_Add( selectedObjElements, SDLGUI_Create_Text( 30, 160, -1, 16, NULL, "      Attack :", 	(int[4]){0,0,0,0}, 			(int[4]){0,0,0,255}, 6, 8, 0, NULL));
	SDLGUI_List_Add( selectedObjElements, SDLGUI_Create_Text( 30, 180, -1, 16, NULL, "     Defence :", 	(int[4]){0,0,0,0}, 			(int[4]){0,0,0,255}, 6, 8, 0, NULL));
	SDLGUI_List_Add( selectedObjElements, textbox_id);
	SDLGUI_List_Add( selectedObjElements, textbox_health);
	SDLGUI_List_Add( selectedObjElements, textbox_maxHealth);
	SDLGUI_List_Add( selectedObjElements, textbox_healthGiven);
	SDLGUI_List_Add( selectedObjElements, textbox_movable);
	SDLGUI_List_Add( selectedObjElements, textbox_pickable);
	SDLGUI_List_Add( selectedObjElements, textbox_attack);
	SDLGUI_List_Add( selectedObjElements, textbox_defence);
	
	SDLGUI_List_Add( bodyItems, selectedObjContainer);
	
	/* top bar */
	topbar = SDLGUI_Create_Panel( GUI_LEFTPANEL_WIDTH, 0, 1280, GUI_TOPBAR_HEIGHT, GUI_UGLY_BGCOLOR, GUI_UGLY_BORDERCOLOR, 4);

	struct SDLGUI_Element *label_mapName = SDLGUI_Create_Text( 10,  0, -1, GUI_TOPBAR_HEIGHT, NO_CLICK_HANDLER, "map:", 	(int[4]){0,0,0,0}, 			(int[4]){0,0,0,255}, 6, 8, 0, NULL);
	struct SDLGUI_Element *textbox_mapName = SDLGUI_Create_Textbox( 40, 8, 23, TEXTBOX_INPUT_NONE, (int[4]){0,0,0,0},  (int[4]){0,0,0,255}, 6, 8, NULL);
	SDLGUI_SetText_Textbox( textbox_mapName, myMap->filePath);
	struct SDLGUI_Element *label_curPos = SDLGUI_Create_Text( 200,  0, -1, GUI_TOPBAR_HEIGHT, NO_CLICK_HANDLER, "pos:", 	(int[4]){0,0,0,0}, 			(int[4]){0,0,0,255}, 6, 8, 0, NULL);
	
	struct SDLGUI_Element *currentPosPanel = SDLGUI_Create_Panel( 230, 0, 80, GUI_TOPBAR_HEIGHT, (int[4]){0,0,0,0}, (int[4]){0,0,0,255}, 0);
	textbox_pos_x = SDLGUI_Create_Textbox( 	00, 8, 3, TEXTBOX_INPUT_NONE, (int[4]){0,0,0,0},  (int[4]){0,0,0,255}, 6, 8, NULL);
	textbox_pos_y = SDLGUI_Create_Textbox( 	40, 8, 3, TEXTBOX_INPUT_NONE, (int[4]){0,0,0,0},  (int[4]){0,0,0,255}, 6, 8, NULL);
	
	struct SDLGUI_List *currentPosElements = SDLGUI_Get_Panel_Elements( currentPosPanel);
	SDLGUI_List_Add( currentPosElements, textbox_pos_x);
	SDLGUI_List_Add( currentPosElements, textbox_pos_y);

	struct SDLGUI_List *topbarItems = SDLGUI_Get_Panel_Elements( topbar);
	SDLGUI_List_Add( topbarItems, label_mapName);
	SDLGUI_List_Add( topbarItems, textbox_mapName);
	SDLGUI_List_Add( topbarItems, label_curPos);
	SDLGUI_List_Add( topbarItems, currentPosPanel);
	
	SDLGUI_Add_Element( bodyContainer);
	SDLGUI_Add_Element( topbar);
}
#undef NO_CLICK_HANDLER
#undef CREATE_LIST_BUTTON
#undef NO_VAR
#undef NO_CHILDREN
#undef NO_FUN

void editor_createMap_clicked( struct SDLGUI_Element *from) {
	int width = parseText( SDLGUI_GetText_Textbox( createMapDialogData.textbox_width  ) );
	int height = parseText( SDLGUI_GetText_Textbox( createMapDialogData.textbox_height) );
	const char *text = SDLGUI_GetText_Textbox( createMapDialogData.textbox_name);
	if( width <= 0 || height <= 0) {
		isMessageBoxOn = true;
		SDLGUI_Show_Message(0, 0, windowW, windowH, SDLGUI_MESSAGE_ERROR, "Width & height must be >= 0");
	}
	else if (strlen(text) < 1) {
		isMessageBoxOn = true;
		SDLGUI_Show_Message(0, 0, windowW, windowH, SDLGUI_MESSAGE_ERROR, "Need a longer map name");
	}
	else {
		createMapDialogData.running = false;
		//TODO I could just dynamically allocate createMapDialogData, then deallocate it here aswell

		myMap = createNewMap( width, height);
		char fileNameBuf[20];
		sprintf( fileNameBuf, "%s.yz.map", text);
		myMap->filePath = strdup( fileNameBuf);
	
		running = true;
	}
}

int setCounter() {
	int maxId = 1;
	int i;
	for( i=0; i<myMap->objListCount; i++) {
		if( myMap->objList[i]->id > maxId)
			maxId = myMap->objList[i]->id;
	}

	return maxId;
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

		if( myMap->objListCount > 0) {
			objectCounter = myMap->objList[ myMap->objListCount-1 ]->id + 1;
		}
	}
	else {
		createMapDialogData.panel = SDLGUI_Create_Panel( 0, 0, windowW, windowH, (int[4]){0,0,0,255}, (int[4]){0,0,0,0}, 0);
		SDLGUI_Add_Element( createMapDialogData.panel);

		struct SDLGUI_List *panelElems = SDLGUI_Get_Panel_Elements( createMapDialogData.panel);
		SDLGUI_List_Add( panelElems, SDLGUI_Create_Text( 0, 100, windowW, 100, NULL, "Creating New Map\n----------------", (int[4]){0,0,0,0}, (int[4]){255,255,255,255}, 12, 16, 0, NULL));
		SDLGUI_List_Add( panelElems, SDLGUI_Create_Text( windowW/2 - 100, 200, -1, 32, NULL, " width", (int[4]){0,0,0,0}, (int[4]){255,255,255,255}, 12, 16, 0, NULL));
		SDLGUI_List_Add( panelElems, SDLGUI_Create_Text( windowW/2 - 100, 230, -1, 32, NULL, "height", (int[4]){0,0,0,0}, (int[4]){255,255,255,255}, 12, 16, 0, NULL));
		SDLGUI_List_Add( panelElems, SDLGUI_Create_Text( windowW/2 - 100, 270, -1, 32, NULL, "  name", (int[4]){0,0,0,0}, (int[4]){255,255,255,255}, 12, 16, 0, NULL));
		SDLGUI_List_Add( panelElems, SDLGUI_Create_Text( windowW/2 - 100, 350, 200, 100, &editor_createMap_clicked, "create", (int[4]){0,0,0,0}, (int[4]){255,255,255,255}, 12, 16, 2, NULL));

		createMapDialogData.textbox_width  = SDLGUI_Create_Textbox( windowW/2, 200, 2, TEXTBOX_INPUT_NUMERIC, (int[4]){255,255,255,50}, (int[4]){255,255,255,255}, 12, 16, NULL);
		createMapDialogData.textbox_height = SDLGUI_Create_Textbox( windowW/2, 230, 2, TEXTBOX_INPUT_NUMERIC, (int[4]){255,255,255,50}, (int[4]){255,255,255,255}, 12, 16, NULL);
		createMapDialogData.textbox_name   = SDLGUI_Create_Textbox( windowW/2, 270, 16, TEXTBOX_INPUT_NUMERIC | TEXTBOX_INPUT_ALPHABET, (int[4]){255,255,255,50}, (int[4]){255,255,255,255}, 12, 16, NULL);
		SDLGUI_List_Add( panelElems, createMapDialogData.textbox_width );
		SDLGUI_List_Add( panelElems, createMapDialogData.textbox_height);
		SDLGUI_List_Add( panelElems, createMapDialogData.textbox_name);
		
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
