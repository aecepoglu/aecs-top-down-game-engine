#include "engine.h"
#include "level-editor/brush.h"
#include "aiTable.h"

#include "sdl2gui/sdl2gui.h"
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
struct SDLGUI_Element *leftPanel;
struct SDLGUI_Element *topBar;
bool mouseDownInGui;
bool isMessageBoxOn = false;


struct {
	struct {
		struct SDLGUI_Element
			*id,
			*health,
			*maxHealth,
			*healthGiven,
			*attack,
			*defence,
			*pos_x,
			*pos_y;
	} textboxes;

	struct {
		struct SDLGUI_Element *pickable, *movable;
	} checkboxes;

	struct SDLGUI_Element *panel;
	struct object *obj;
} selectedObjStuff = {
	.obj = NULL,
};

struct {
	struct SDLGUI_Element *panel, *textbox_width, *textbox_height, *textbox_name;
	bool running;
} createMapDialogData;

#define SHOW_TOOLTIP( x, y, text) SDLGUI_Show_Tooltip( x*TILELEN + GUI_LEFTPANEL_WIDTH, y*TILELEN + GUI_TOPBAR_HEIGHT, text)


/* ---------------------------
	Brush Functions
*/

bool drawTerrain( unsigned int x, unsigned int y, int type){
	if( myMap->tiles[x][y] != type && myMap->objs[x][y]==NULL) {
		myMap->tiles[x][y] = type;
		return true;
	}
	else
		return false;
}

bool editor_selectObj( unsigned int x, unsigned int y, int type) {
	selectedObjStuff.obj = myMap->objs[x][y];

	char tmp[4];
	if( selectedObjStuff.obj != NULL) {
		selectedObjStuff.panel->isVisible = true;

    	sprintf(tmp, "%d", selectedObjStuff.obj->id);
		SDLGUI_SetText_Textbox( selectedObjStuff.textboxes.id, tmp);

		sprintf(tmp, "%d", selectedObjStuff.obj->health);
		SDLGUI_SetText_Textbox( selectedObjStuff.textboxes.health, tmp);

		sprintf(tmp, "%d", selectedObjStuff.obj->maxHealth);
		SDLGUI_SetText_Textbox( selectedObjStuff.textboxes.maxHealth, tmp);

		sprintf(tmp, "%d", selectedObjStuff.obj->healthGiven);
		SDLGUI_SetText_Textbox( selectedObjStuff.textboxes.healthGiven, tmp);

		sprintf(tmp, "%d", selectedObjStuff.obj->attack);
		SDLGUI_SetText_Textbox( selectedObjStuff.textboxes.attack, tmp);

		sprintf(tmp, "%d", selectedObjStuff.obj->defence);
		SDLGUI_SetText_Textbox( selectedObjStuff.textboxes.defence, tmp);
		
		SDLGUI_Checkbox_SetValue( selectedObjStuff.checkboxes.movable, selectedObjStuff.obj->isMovable);

		SDLGUI_Checkbox_SetValue( selectedObjStuff.checkboxes.pickable, selectedObjStuff.obj->isPickable);
	}
	else {
		selectedObjStuff.panel->isVisible = false;
	}

	sprintf( tmp, "%d", x);
	SDLGUI_SetText_Textbox( selectedObjStuff.textboxes.pos_x, tmp);

	sprintf( tmp, "%d", y);
	SDLGUI_SetText_Textbox( selectedObjStuff.textboxes.pos_y, tmp);
	return true;
}

bool editor_createObj( unsigned int x, unsigned int y, int type){
	//add objects only if there is no object there
	if( myMap->objs[x][y]!=NULL || myMap->tiles[x][y]==terrain_wall)
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

		struct AI *ai = AI_CREATE( type); //TODO don't create the whole AI, just create a placeholder
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

int parseText( const char *text) {
	if( strlen(text) == 0)
		return 0;
	else
		return atoi( text);
}

void textbox_id_changed( struct SDLGUI_Element *textbox, const char *text) {
	if( selectedObjStuff.obj && strlen(text) > 0) {
		int newId = atoi( text);
		if( newId >= 1) {
			selectedObjStuff.obj->id = newId;
		}
		else {
			SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_ERROR, "You must give id >= 1");
			isMessageBoxOn = true;
		}
	}
}

void textbox_health_changed( struct SDLGUI_Element *textbox, const char *text) {
	if( selectedObjStuff.obj) {
		int newHealth = parseText( text);
		if( newHealth <= UINT8_MAX) {
			selectedObjStuff.obj->health = newHealth;
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
	if( selectedObjStuff.obj) {
		int newHealth = parseText( text);
		if( newHealth <= UINT8_MAX) {
			selectedObjStuff.obj->maxHealth = newHealth;
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
	if( selectedObjStuff.obj) {
		int newValue = parseText( text);
		if( newValue <= INT8_MAX && newValue >= INT8_MIN) {
			selectedObjStuff.obj->healthGiven = newValue;
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

void checkbox_movable_changed( struct SDLGUI_Element *checkbox, int value) {
	if( selectedObjStuff.obj) {
		selectedObjStuff.obj->isMovable = value;
	}
	else {
		SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_WARNING, "No object selected");
		isMessageBoxOn = true;
	}
}

void checkbox_pickable_changed( struct SDLGUI_Element *checkbox, int value) {
	if( selectedObjStuff.obj) {
		selectedObjStuff.obj->isPickable = value;
	}
	else {
		SDLGUI_Show_Message( 0, 0, windowW, windowH, SDLGUI_MESSAGE_WARNING, "No object selected");
		isMessageBoxOn = true;
	}
}

void textbox_attack_changed( struct SDLGUI_Element *textbox, const char *text) {
	if( selectedObjStuff.obj) {
		int newValue = parseText( text);
		if( newValue <= UINT8_MAX ) {
			selectedObjStuff.obj->attack = newValue;
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
	if( selectedObjStuff.obj) {
		int newValue = parseText( text);
		if( newValue <= UINT8_MAX ) {
			selectedObjStuff.obj->defence = newValue;
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
	if( matched) {
		switch (e->keysym.sym) {
			case SDLK_s:
				buttonSave_clicked(NULL);
				break;
			case SDLK_q:
				buttonQuit_clicked(NULL);
				break;
		};
	}

	if( matched) {
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

	return ( brush.fun != NULL && x < myMap->width && y < myMap->height && brush.fun( x, y, brush.variant) );
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
			leftPanel->rect.h = e->data2;
			topBar->rect.w = e->data1 - GUI_LEFTPANEL_WIDTH;
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
					break;
				}
				else {
					SDLGUI_Handle_MouseDown( (SDL_MouseButtonEvent*)&e);
					continue;
				}
			case SDL_MOUSEBUTTONUP:
				if( isMessageBoxOn) {
					SDLGUI_Hide_Message();
					isMessageBoxOn =false;
				}
				else {
					SDLGUI_Handle_MouseUp( (SDL_MouseButtonEvent*)&e);
				}
				break;
			case SDL_MOUSEMOTION:
				if (SDLGUI_Handle_MouseHover( (SDL_MouseMotionEvent*)&e))
					break;
				else
					continue;
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
				if (SDLGUI_Handle_MouseHover( (SDL_MouseMotionEvent*)&e)) {
					break;
				}
				else if( mouseDownInGui != true && handleMouse( 0, (SDL_MouseMotionEvent*)&e) ) {
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
				if( obj == selectedObjStuff.obj) {
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
	SDL_RenderClear( renderer);

	drawTexture( renderer, bgroundTexture, GUI_LEFTPANEL_WIDTH, GUI_TOPBAR_HEIGHT, viewSize.i*TILELEN, viewSize.j*TILELEN);
	drawObjects( );

	SDLGUI_Draw();

	SDL_RenderPresent( renderer);
}



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
	//return CREATE_LIST_BUTTON( lineNo, buttonText, CREATE_BRUSH_WRAPPER( SDLK_1 + lineNo, applyObjTemplate, lineNo, NO_CHILDREN));
	return NULL;
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

void hideAll() { //TODO delete this function, and use focus-lost instead
	brushOptionPanels.rotate->isVisible = false;
	brushOptionPanels.terrain->isVisible = false;
	brushOptionPanels.object->isVisible = false;
	brushOptionPanels.ai->isVisible = false;
}

void button_select_clicked( struct SDLGUI_Element *e) {
	brush.fun = editor_selectObj;
	hideAll();
}

void button_rotate_clicked( struct SDLGUI_Element *e) {
	brush.fun = setDirection;
	brush.variant = dir_up;
	hideAll();
	brushOptionPanels.rotate->isVisible = true;
}

void button_terrain_clicked( struct SDLGUI_Element *e) {
	brush.fun = drawTerrain;
	brush.variant = terrain_wall;
	hideAll();
	brushOptionPanels.terrain->isVisible = true;
}

void button_object_clicked( struct SDLGUI_Element *e) {
	brush.fun = editor_createObj;
	brush.variant = go_apple;
	hideAll();
	brushOptionPanels.object->isVisible = true;
}

void button_ai_clicked( struct SDLGUI_Element *e) {
	brush.fun = drawAI;
	brush.variant = ai_none;
	hideAll();
	brushOptionPanels.ai->isVisible = true;
}

void initGui() {
    /* The left panel */
	SDLGUI_Color panelsBgColor = (SDLGUI_Color){.r=170, .g=180, .b=190, .a=255};
	SDLGUI_Color panelsBorderColor = (SDLGUI_Color){.r=255, .g=0, .b=0, .a=255};
	SDLGUI_Params panelParams = {.bgColor=panelsBgColor, .fgColor=panelsBorderColor, .borderThickness=1};
	SDLGUI_Params labelParams = (SDLGUI_Params){
		.bgColor=COLOR_TRANSPARENT,
		.fgColor=COLOR_BLACK,
		.fontWidth=6,
		.fontHeight=8
	};
	SDLGUI_Params buttonParams = (SDLGUI_Params){
		.bgColor=COLOR_TRANSPARENT,
		.fgColor=COLOR_BLACK,
		.fontWidth=6,
		.fontHeight=8,
		.borderThickness=1
	};

	leftPanel = SDLGUI_Create_Panel( (SDL_Rect){.x=0, .y=0, .w=GUI_LEFTPANEL_WIDTH, .h=960}, panelParams);

	/* top bar */
	topBar = SDLGUI_Create_Panel( (SDL_Rect){.x=GUI_LEFTPANEL_WIDTH, .y=0, .w=1280-GUI_LEFTPANEL_WIDTH, .h=GUI_TOPBAR_HEIGHT}, panelParams);

	SDLGUI_Add_Element( leftPanel);
	SDLGUI_Add_Element( topBar);


	SDLGUI_AddTo_Panel( leftPanel, SDLGUI_Create_Text( (SDL_Rect){.x=0, .y=0, .w=GUI_LEFTPANEL_WIDTH}, " YZ-01 \n-------", (SDLGUI_Params){
			.bgColor=labelParams.bgColor,
			.fgColor=labelParams.fgColor,
			.fontWidth=18,
			.fontHeight=24
		}
	));


	struct SDLGUI_Element *buttonsContainer = SDLGUI_Create_Panel( (SDL_Rect){.x=10, .y=100, .w=GUI_LEFTPANEL_WIDTH-2*10, .h=200}, panelParams);
	SDLGUI_AddTo_Panel( leftPanel, buttonsContainer);

	struct buttonTemplate {
		char *fileName;
		SDLGUI_Clicked *onClick;
	};
	struct buttonTemplate buttonTemplates[] = {
		{"res/editor/select.png", 	button_select_clicked},
		{"res/editor/rotate.png", 	button_rotate_clicked},
		{"res/editor/terrain.png",	button_terrain_clicked},
		{"res/editor/object.png", 	button_object_clicked},
		{"res/editor/ai.png", 		button_ai_clicked},
	};
	
	int i;
	const int buttonsPerRow=3;
	const int buttonSize = 32;
	const int iconSize = 16;
	const int buttonSizeWithMargins = 48;
	for( i=0; i<sizeof(buttonTemplates) / sizeof(struct buttonTemplate); i++) {
		struct buttonTemplate *template = &buttonTemplates[i];

		SDL_Texture *texture = loadTexture( renderer, template->fileName);
		
		struct SDLGUI_Element *element = SDLGUI_Create_Texture( (SDL_Rect){.x=(i%buttonsPerRow) *buttonSizeWithMargins, .y=(i/buttonsPerRow) *buttonSizeWithMargins, .w=buttonSize, .h=buttonSize}, texture, iconSize, iconSize, buttonParams);

		SDL_DestroyTexture( texture);
		
		element->clicked = template->onClick;
		SDLGUI_AddTo_Panel( buttonsContainer, element);
	}

	struct SDLGUI_Element *brushOptionsContainer = SDLGUI_Create_Panel( (SDL_Rect){.x=buttonsContainer->rect.x, .y=370, .w=buttonsContainer->rect.w, .h=250}, (SDLGUI_Params) {
			.bgColor = COLOR_TRANSPARENT,
			.borderThickness = 0
		}
	);
	SDLGUI_AddTo_Panel( leftPanel, brushOptionsContainer);

	brushOptionPanels_init( brushOptionsContainer, textures->obj, textures->trn);

	
	selectedObjStuff.panel = SDLGUI_Create_Panel( (SDL_Rect){.x=buttonsContainer->rect.x, .y=700, .w=buttonsContainer->rect.w, .h=200}, panelParams);
	selectedObjStuff.panel->isVisible = false;
	SDLGUI_AddTo_Panel( leftPanel, selectedObjStuff.panel);

	char mapText[256];
	sprintf( mapText, "map: %s", myMap->filePath);
	SDLGUI_AddTo_Panel( topBar, SDLGUI_Create_Text( (SDL_Rect){.x= 150, .y=0, .w=TEXT_SPAN_SIZE, .h=GUI_TOPBAR_HEIGHT}, mapText, labelParams));

	struct itemTemplate {
		char *label;
		struct SDLGUI_Element **elementPtr;
		SDLGUI_TextBox_Changed *textChangeCallback;
		SDLGUI_Checkbox_Changed *checkboxChangedCallback;
	};
	struct itemTemplate itemsToCreate[] = {
		{ "          id", 	&selectedObjStuff.textboxes.id			, textbox_id_changed 			, NULL 	},
		{ "      health", 	&selectedObjStuff.textboxes.health		, textbox_health_changed 		, NULL 	},
		{ "  max health", 	&selectedObjStuff.textboxes.maxHealth 	, textbox_maxHealth_changed 	, NULL 	},
		{ "health given", 	&selectedObjStuff.textboxes.healthGiven	, textbox_healthGiven_changed 	, NULL 	},
		{ "  is movable", 	&selectedObjStuff.checkboxes.movable	, NULL							, checkbox_movable_changed 	},
		{ " is pickable", 	&selectedObjStuff.checkboxes.pickable	, NULL							, checkbox_pickable_changed },
		{ "         ATK", 	&selectedObjStuff.textboxes.attack		, textbox_attack_changed 		, NULL 	},
		{ "         DEF", 	&selectedObjStuff.textboxes.defence		, textbox_defence_changed 		, NULL 	},
	};
	SDLGUI_Params textboxParams = labelParams;
	textboxParams.bgColor = COLOR_WHITE;
	textboxParams.borderThickness = 1;

	const int textboxPairHeight = 15;
	for( i=0; i<8; i++) {
		struct itemTemplate *template = &itemsToCreate[i];
		int yVal = 5+i*textboxPairHeight;

		struct SDLGUI_Element *label = SDLGUI_Create_Text( (SDL_Rect){.x=10, .y=yVal, .w=80, .h=textboxPairHeight}, template->label, labelParams);
		struct SDLGUI_Element *value;
		
		if ( template->textChangeCallback != NULL) {
			value = SDLGUI_Create_Textbox( (SDL_Rect){.x=90, .y=yVal, .w=80, .h=textboxPairHeight}, textboxParams);
			value->data.textData->acceptedChars  = TEXTBOX_INPUT_NUMERIC;
			value->data.textData->textChanged = template->textChangeCallback;
		}
		else {
			value = SDLGUI_Create_Checkbox( (SDL_Rect){.x=90, .y=yVal, .w=0, .h=textboxPairHeight}, textboxParams);
			SDLGUI_Checkbox_SetOnCheckChanged( value, template->checkboxChangedCallback);
		}

		SDLGUI_AddTo_Panel( selectedObjStuff.panel, label);
		SDLGUI_AddTo_Panel( selectedObjStuff.panel, value);
		
		*template->elementPtr = value;
	}


	labelParams.fontWidth *= 2;
	labelParams.fontHeight *= 2;

	selectedObjStuff.textboxes.pos_x = SDLGUI_Create_Textbox( (SDL_Rect){.x=20, .y=0, .w=40, .h=GUI_TOPBAR_HEIGHT}, labelParams);
	selectedObjStuff.textboxes.pos_y = SDLGUI_Create_Textbox( (SDL_Rect){.x=60, .y=0, .w=40, .h=GUI_TOPBAR_HEIGHT}, labelParams);
	selectedObjStuff.textboxes.pos_x->data.textData->acceptedChars = TEXTBOX_INPUT_NONE;
	selectedObjStuff.textboxes.pos_y->data.textData->acceptedChars = TEXTBOX_INPUT_NONE;
	SDLGUI_AddTo_Panel( topBar, selectedObjStuff.textboxes.pos_x );
	SDLGUI_AddTo_Panel( topBar, selectedObjStuff.textboxes.pos_y );
}

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

void mapDialog_quit_clicked( struct SDLGUI_Element *from) {
	//createMapDialogData.running = false;
	printf("click callback\n");
}

int main( int argc, char *args[]) {
	//Default values
	myMap = 0;

	init( GUI_LEFTPANEL_WIDTH, GUI_TOPBAR_HEIGHT, 1280, 960);
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
		SDLGUI_Params labelParams = (SDLGUI_Params){ .bgColor=COLOR_TRANSPARENT, .fgColor=COLOR_WHITE, .fontWidth=12, .fontHeight=16 };

		SDLGUI_Params buttonParams = labelParams;
		buttonParams.borderThickness = 2;

		createMapDialogData.panel = SDLGUI_Create_Panel( (SDL_Rect){.x=100, .y=100, .w=300, .h=500}, (SDLGUI_Params){.bgColor=COLOR_BLACK} );
		
		struct SDLGUI_Element *button_createMap = SDLGUI_Create_Text( (SDL_Rect){.x=0, .y=250, .w=200, .h=100}, "create", buttonParams);
		struct SDLGUI_Element *button_quit = SDLGUI_Create_Text( (SDL_Rect){.x=0, .y=400, .w=200, .h=50}, "quit", buttonParams);
		button_createMap->clicked = editor_createMap_clicked;
		button_quit->clicked = mapDialog_quit_clicked;
		
		createMapDialogData.textbox_width  = SDLGUI_Create_Textbox( (SDL_Rect){.x=100, .y=100, .w=100, .h=30}, labelParams);
		createMapDialogData.textbox_height = SDLGUI_Create_Textbox( (SDL_Rect){.x=100, .y=130, .w=100, .h=30}, labelParams);
		createMapDialogData.textbox_name   = SDLGUI_Create_Textbox( (SDL_Rect){.x=100, .y=170, .w=100, .h=30}, labelParams);
		createMapDialogData.textbox_width->data.textData->acceptedChars  = TEXTBOX_INPUT_NUMERIC;
		createMapDialogData.textbox_height->data.textData->acceptedChars = TEXTBOX_INPUT_NUMERIC;
		createMapDialogData.textbox_name->data.textData->acceptedChars   = TEXTBOX_INPUT_NUMERIC | TEXTBOX_INPUT_ALPHABET;
		
		SDLGUI_AddTo_Panel( createMapDialogData.panel, SDLGUI_Create_Text( (SDL_Rect){.x=0, .y= 10, .h=30}, "Creating New Map\n----------------", labelParams) );
		SDLGUI_AddTo_Panel( createMapDialogData.panel, SDLGUI_Create_Text( (SDL_Rect){.x=0, .y=100, .h=30}, " width", labelParams) );
		SDLGUI_AddTo_Panel( createMapDialogData.panel, SDLGUI_Create_Text( (SDL_Rect){.x=0, .y=130, .h=30}, "height", labelParams) );
		SDLGUI_AddTo_Panel( createMapDialogData.panel, SDLGUI_Create_Text( (SDL_Rect){.x=0, .y=170, .h=30}, "  name", labelParams) );
		
		SDLGUI_AddTo_Panel( createMapDialogData.panel, createMapDialogData.textbox_width );
		SDLGUI_AddTo_Panel( createMapDialogData.panel, createMapDialogData.textbox_height);
		SDLGUI_AddTo_Panel( createMapDialogData.panel, createMapDialogData.textbox_name);
		
		SDLGUI_AddTo_Panel( createMapDialogData.panel, button_createMap );
		SDLGUI_AddTo_Panel( createMapDialogData.panel, button_quit );

		SDLGUI_Add_Element( createMapDialogData.panel);

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
