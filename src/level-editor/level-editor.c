#include "levelEditor.h"
#include "editor-brushes/brush.h"
#include "editor-brushes/texture.h"

#include "../collection/stack.h"
#include "templates.h"
#include "../util/error.h"
#include "dialogs/mapCreator.h"
#include "guiHelpers.h"
#include "../texture/spriteSpecs.h"

bool running;
unsigned int objectCounter = 1;

bool moveForward( struct Map *map, struct object* obj) { return false; }
bool turnLeft( struct Map *map, struct object *obj) { return false; }
bool turnRight( struct Map *map, struct object* obj) { return false; }

/* GUI Elements */
#define GUI_LEFTPANEL_WIDTH 192
#define GUI_TOPBAR_HEIGHT 32
bool isMessageBoxOn = false;
struct SpriteSpecsList *spriteSpecs = NULL;


struct {
	struct {
		struct SDLGUI_Element
			*id,
			*health,
			*maxHealth,
			*attack,
			*defence,
			*ai,
			*pos_x,
			*pos_y;
	} textboxes;

	struct {
		struct SDLGUI_Element *pickable, *movable;
	} checkboxes;

	struct SDLGUI_Element *panel;
	struct object *obj;
} selectedObjStuff;

#define SHOW_TOOLTIP( x, y, text) if(x>=viewPos.i && y>=viewPos.j && x<viewEnd.i && y<viewEnd.j) SDLGUI_Show_Tooltip( (x-viewPos.i)*TILELEN + GUI_LEFTPANEL_WIDTH, (y-viewPos.j)*TILELEN + GUI_TOPBAR_HEIGHT, text)

/* --------------------------
	Helper Functions
*/

bool checkMapIfClear (unsigned int x0, unsigned int y0, unsigned int w, unsigned int h, const struct object *ignoredObj) {
	int x,y;
	for (x = x0; x < MIN(myMap->width, x0 + w); x++) {
		for (y = y0; y < MIN(myMap->height, y0+h); y++) {
			if (myMap->tiles[x][y] == terrain_wall ||
			    (myMap->objs[x][y] != NULL && myMap->objs[x][y] != ignoredObj)) {
			    	printf("unclear at %d,%d\n", x, y);

			    	return false;
			}
		}
	}
	return true;
}

void unsetObjectMap(struct Map *map, const struct object *obj) {
	int i, j;

	for (i=obj->pos.i; i < obj->pos.i + obj->width; i++)
		for (j=obj->pos.j; j < obj->pos.j + obj->height; j ++)
			map->objs[i][j] = NULL;
}

void setObjectMap(struct Map *map, struct object *obj) {
	int i, j;

	for (i=obj->pos.i; i < obj->pos.i + obj->width; i++)
		for (j=obj->pos.j; j < obj->pos.j + obj->height; j ++)
			map->objs[i][j] = obj;
}

bool editor_selectObj (unsigned int x, unsigned int y) {
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

		sprintf(tmp, "%d", selectedObjStuff.obj->attack);
		SDLGUI_SetText_Textbox( selectedObjStuff.textboxes.attack, tmp);

		sprintf(tmp, "%d", selectedObjStuff.obj->defence);
		SDLGUI_SetText_Textbox( selectedObjStuff.textboxes.defence, tmp);

		if (selectedObjStuff.obj->ai != NULL) {
			/* replace ' ' with '\n' in ai name so it fits in the textbox */
			char *name = aiNames[selectedObjStuff.obj->ai->type];
			int len = strlen(name);
			int i;
			char *newName = calloc(len + 1, sizeof(char));
			for( i=0; i<len; i++)
				newName[i] = name[i] != ' ' ? name[i] : '\n';

			SDLGUI_SetText_Textbox( selectedObjStuff.textboxes.ai, newName);

			free( newName);
		}
		else
			SDLGUI_SetText_Textbox( selectedObjStuff.textboxes.ai, aiNames[ai_none]);

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



/* ---------------------------
	Brush Functions
*/

bool editor_removeObject( unsigned int x, unsigned int y, int type) {
	if( myMap->objs[x][y] != 0) {
		myMap->objs[x][y]->isDeleted = true;
		myMap->objs[x][y] = NULL;
		return true;
	}
	else
		return false;
}

bool editor_applyTemplate( unsigned int x, unsigned int y, int type) {
	if( myMap->objs[x][y] != 0
	 && template_apply( myMap->objs[x][y], type)
	) {
		editor_selectObj(x, y);

		SHOW_TOOLTIP( x, y, "Updated");

		return true;
	}
	else
		return false;
}

bool editor_drawTerrain( unsigned int x, unsigned int y, int type){
	if( myMap->tiles[x][y] != type && myMap->objs[x][y]==NULL) {
		myMap->tiles[x][y] = type;
		return true;
	}
	else
		return false;
}

bool editor_createObj( unsigned int x, unsigned int y, int type){
	//add objects only if there is no object there
	if( myMap->tiles[x][y]==terrain_wall)
		return false;

	if( myMap->objs[x][y]!=NULL )
		return false;

	struct object *obj = createObject( go_npc, x, y, objectCounter++, -1);


	addObject( obj, myMap, x, y);
	editor_selectObj(x, y);

	char tooltipText[8];
	sprintf( tooltipText, "#%d", obj->id);
	SHOW_TOOLTIP( x, y, tooltipText);

	return true;
}

bool editor_setTexture( unsigned int x, unsigned int y, int textureId){
	if( myMap->objs[x][y] == NULL) {
		return false;
	}

	myMap->objs[x][y]->textureId = textureId;

	return true;
}

bool editor_drawAI( unsigned int x, unsigned int y, int type) {
	//if there is an object at the given location, and it dsoen't have an AI
	if( myMap->objs[x][y] != 0) {
		if ( myMap->objs[x][y]->ai != NULL) {
			myMap->objs[x][y]->ai->type = type;
		}
		else {
			struct AI *ai = (struct AI*)malloc( sizeof( struct AI));
			ai->type = type;
			myMap->objs[x][y]->ai = ai;
		}
		editor_selectObj(x, y);
		SHOW_TOOLTIP( x, y, "AI put");
		return true;
	}

	return false;
}

bool editor_move( unsigned int x, unsigned int y, int type) {
	if( brush.isRepeat ) {
		int oldX = selectedObjStuff.obj->pos.i;
		int oldY = selectedObjStuff.obj->pos.j;
		if ((x != oldX || y != oldY) &&
		    checkMapIfClear(x, y,
		                    selectedObjStuff.obj->width,
		                    selectedObjStuff.obj->height,
				    selectedObjStuff.obj)) {

			unsetObjectMap(myMap, selectedObjStuff.obj);

			selectedObjStuff.obj->pos.i = x;
			selectedObjStuff.obj->pos.j = y;

			setObjectMap(myMap, selectedObjStuff.obj);

			return true;
		}
	}
	else {
		editor_selectObj(x, y);

		if( selectedObjStuff.obj != NULL)
			brush.isRepeat = true;
	}
	return false;
}

bool editor_rotate( unsigned int x, unsigned int y, int type) {
	if( brush.isRepeat ) {
		enum direction newDir = vector_dirTan( y - selectedObjStuff.obj->pos.j, x - selectedObjStuff.obj->pos.i);

		int newWidth, newHeight;

		if ((newDir - selectedObjStuff.obj->dir + 4) %2 != 0) {
			newWidth = selectedObjStuff.obj->height;
			newHeight = selectedObjStuff.obj->width;
		}
		else {
			newWidth = selectedObjStuff.obj->width;
			newHeight = selectedObjStuff.obj->height;
		}

		if ((x != selectedObjStuff.obj->pos.i || y != selectedObjStuff.obj->pos.j) &&
		    selectedObjStuff.obj->dir != newDir && 
		    checkMapIfClear(selectedObjStuff.obj->pos.i,
		                    selectedObjStuff.obj->pos.j,
				    newWidth, newHeight,
				    selectedObjStuff.obj)) {

			selectedObjStuff.obj->dir = newDir;

			unsetObjectMap(myMap, selectedObjStuff.obj);

			selectedObjStuff.obj->width = newWidth;
			selectedObjStuff.obj->height = newHeight;

			setObjectMap(myMap, selectedObjStuff.obj);

			return true;
		}
	}
	else {
		selectedObjStuff.obj = myMap->objs[x][y];

		if( selectedObjStuff.obj != NULL) {
			brush.isRepeat = true;
			return true;
		}
	}
	return false;
}

bool editor_resize( unsigned int x, unsigned int y, int type) {
	if (brush.isRepeat) {
		int width = x - selectedObjStuff.obj->pos.i + 1;
		int height = y - selectedObjStuff.obj->pos.j + 1;

		if ((width != selectedObjStuff.obj->width ||
		     height != selectedObjStuff.obj->height) && 
		    width > 0 && height > 0 &&
		    checkMapIfClear(selectedObjStuff.obj->pos.i,
		                    selectedObjStuff.obj->pos.j,
				    width, height,
				    selectedObjStuff.obj)) {

			unsetObjectMap(myMap, selectedObjStuff.obj);

			selectedObjStuff.obj->width = width;
			selectedObjStuff.obj->height = height;

			setObjectMap(myMap, selectedObjStuff.obj);

			return true;
		}
	}
	else {
		selectedObjStuff.obj = myMap->objs[x][y];

		if (selectedObjStuff.obj != NULL) {
			brush.isRepeat = true;
			return true;
		}
	}
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
	log3("generate background end\n");
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
	SDLGUI_Show_Message(msgType, msgText);
}

void textbox_id_changed( struct SDLGUI_Element *textbox, const char *text) {
	if( selectedObjStuff.obj && strlen(text) > 0) {
		int newId = atoi( text);
		if( newId >= 1) {
			selectedObjStuff.obj->id = newId;
		}
		else {
			SDLGUI_Show_Message( SDLGUI_MESSAGE_ERROR, "You must give id >= 1");
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
			SDLGUI_Show_Message( SDLGUI_MESSAGE_ERROR, "Health should be <= 256");
			isMessageBoxOn = true;
		}
	}
	else {
		SDLGUI_Show_Message( SDLGUI_MESSAGE_WARNING, "No object selected");
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
			SDLGUI_Show_Message( SDLGUI_MESSAGE_ERROR, "Max-Health should be <= 256");
			isMessageBoxOn = true;
		}
	}
	else {
		SDLGUI_Show_Message( SDLGUI_MESSAGE_WARNING, "No object selected");
		isMessageBoxOn = true;
	}
}

void checkbox_movable_changed( struct SDLGUI_Element *checkbox, int value) {
	if( selectedObjStuff.obj) {
		selectedObjStuff.obj->isMovable = value;
	}
	else {
		SDLGUI_Show_Message( SDLGUI_MESSAGE_WARNING, "No object selected");
		isMessageBoxOn = true;
	}
}

void checkbox_pickable_changed( struct SDLGUI_Element *checkbox, int value) {
	if( selectedObjStuff.obj) {
		selectedObjStuff.obj->isPickable = value;
	}
	else {
		SDLGUI_Show_Message( SDLGUI_MESSAGE_WARNING, "No object selected");
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
			SDLGUI_Show_Message( SDLGUI_MESSAGE_ERROR, "Attack can be <= 256");
			isMessageBoxOn = true;
		}
	}
	else {
		SDLGUI_Show_Message( SDLGUI_MESSAGE_WARNING, "No object selected");
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
			SDLGUI_Show_Message( SDLGUI_MESSAGE_ERROR, "Defence can be <= 256");
			isMessageBoxOn = true;
		}
	}
	else {
		SDLGUI_Show_Message( SDLGUI_MESSAGE_WARNING, "No object selected");
		isMessageBoxOn = true;
	}
}

void handleKey( SDL_KeyboardEvent *e) {
	switch( e->keysym.sym) {
		case SDLK_s:
	        if( e->repeat != true)
			    buttonSave_clicked(NULL);
			break;
		case SDLK_q:
	        if( e->repeat != true)
			    buttonQuit_clicked(NULL);
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
	};
}

/* The mouse button-event and motion-events are handled the same
 * I just need to extract the x,y from the event-data.
 */
int handleMouseGeneric( int x, int y) {
	x = x / TILELEN + viewPos.i;
	y = y / TILELEN + viewPos.j;

	if ( brush.fun != NULL && x < myMap->width && y < myMap->height && brush.fun( x, y, brush.variant) ) {
		drawBackground();
		return 1;
	}

	return 0;
}
void handleMouseButton( SDL_MouseButtonEvent *e) {
	handleMouseGeneric( e->x, e->y);
}

int handleMouseMotion( SDL_MouseMotionEvent *e) {

	if( ! e->state) //if no buttons are pressed
		return 0;

	return handleMouseGeneric(e->x, e->y);
}

void handleResize(const SDL_Rect *r) {
	resizeView( 0, 0, r->w, r->h);
	drawBackground();
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
				e->windowID, e->data1, e->data2);

			SDLGUI_Resize(e->data1, e->data2);
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

void run() {
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
				if(SDLGUI_Handle_TextInput( (SDL_KeyboardEvent*)&e) != true)
					handleKey( (SDL_KeyboardEvent*)&e);
				break;
			case SDL_MOUSEBUTTONDOWN:
				SDLGUI_Handle_MouseDown( (SDL_MouseButtonEvent*)&e);
				break;
			case SDL_MOUSEBUTTONUP:
				SDLGUI_Handle_MouseUp( (SDL_MouseButtonEvent*)&e);
				brush.isRepeat = false;
				break;
			case SDL_MOUSEWHEEL:
				if (SDLGUI_Handle_Scroll( (SDL_MouseWheelEvent*)&e))
					break;
				else
					continue;
			case SDL_MOUSEMOTION:
				if(SDLGUI_Handle_MouseHover( (SDL_MouseMotionEvent*)&e))
					break;
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
void drawObjects( SDL_Rect *r) {
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
					(obj->textureId < 0 || obj->textureId >= textures->objsCount || textures->obj[obj->textureId] == NULL)
						? textures->unidentifiedObj
						: textures->obj[obj->textureId]->textures[ obj->visualState][obj->dir],
					screenPos.i*TILELEN + r->x, screenPos.j*TILELEN + r->y,
					TILELEN * obj->width, TILELEN * obj->height );

				if( obj == selectedObjStuff.obj) {
					drawTexture (renderer, textures->highlitObjIndicator,
					             screenPos.i*TILELEN + GUI_LEFTPANEL_WIDTH, screenPos.j*TILELEN + GUI_TOPBAR_HEIGHT,
						     TILELEN * selectedObjStuff.obj->width, TILELEN * selectedObjStuff.obj->height);
				}
			}
		}
	}

	free( myMap->objList);
	myMap->objList = newObjList;
	myMap->objListCount = newCount;

}

void drawCanvas( SDL_Rect *r) {
	drawTexture( renderer, bgroundTexture, r->x, r->y, viewSize.i*TILELEN, viewSize.j*TILELEN);
	drawObjects( r);
}

void draw() {
	SDL_RenderClear( renderer);

	SDLGUI_Draw();

	SDL_RenderPresent( renderer);
}



void hideAll() {
	brushOptionPanels.move->isVisible = false;
	brushOptionPanels.rotate->isVisible = false;
	brushOptionPanels.terrain->isVisible = false;
	brushOptionPanels.ai->isVisible = false;
	brushOptionPanels.templates->isVisible = false;
	brushOptionPanels.texture->isVisible = false;
}

void button_move_clicked( struct SDLGUI_Element *e) {
	brush.fun = editor_move;
	brush.variant = 0;
	hideAll();
	brushOptionPanels.move->isVisible = true;
}

void button_rotate_clicked( struct SDLGUI_Element *e) {
	brush.fun = editor_rotate;
	brush.variant = 0;
	hideAll();
	brushOptionPanels.rotate->isVisible = true;
}

void button_resize_clicked( struct SDLGUI_Element *e) {
	brush.fun = editor_resize;
	hideAll();
}

void button_terrain_clicked( struct SDLGUI_Element *e) {
	brush.fun = editor_drawTerrain;
	brush.variant = terrain_wall;
	hideAll();
	brushOptionPanels.terrain->isVisible = true;
}

void button_object_clicked( struct SDLGUI_Element *e) {
	brush.fun = editor_createObj;
	brush.variant = go_npc;
	hideAll();
}

void button_ai_clicked( struct SDLGUI_Element *e) {
	brush.fun = editor_drawAI;
	brush.variant = ai_none;
	hideAll();
	brushOptionPanels.ai->isVisible = true;
}

void button_remove_clicked( struct SDLGUI_Element *e) {
	brush.fun = editor_removeObject;
	hideAll();
}

void button_template_clicked( struct SDLGUI_Element *e) {
	brush.fun = editor_move;
	brush.variant = 0;
	hideAll();
	brushOptionPanels.templates->isVisible = true;
}

void button_texture_clicked( struct SDLGUI_Element *e) {
	brush.fun = editor_setTexture;
	brush.variant = -1;
	hideAll();
	brushOptionPanels.texture->isVisible = true;
}


void initGui() {
    /* The left panel */
	SDLGUI_Color panelsBgColor = (SDLGUI_Color){.r=170, .g=180, .b=190, .a=255};
	SDLGUI_Params panelParams = {.bgColor=panelsBgColor, .fgColor=COLOR_BLACK, .borderThickness=1};
	SDLGUI_Params labelParams = (SDLGUI_Params){
		.bgColor=COLOR_TRANSPARENT,
		.fgColor=COLOR_BLACK,
		.fontWidth=6,
		.fontHeight=8
	};
	SDLGUI_Params buttonParams = (SDLGUI_Params){
		.bgColor=COLOR_TRANSPARENT,
		.fgColor=COLOR_BLACK,
		.fontWidth=12,
		.fontHeight=16,
		.borderThickness=1
	};

	struct SDLGUI_Element *leftPanel = SDLGUI_Create_Panel( (SDL_Rect){.x=0, .y=0, .w=GUI_LEFTPANEL_WIDTH, .h=SDLGUI_SIZE_FILL}, panelParams);

	/* top bar */
	struct SDLGUI_Element *topBar = SDLGUI_Create_Panel( (SDL_Rect){.x=GUI_LEFTPANEL_WIDTH, .y=0, .w=SDLGUI_SIZE_FILL, .h=GUI_TOPBAR_HEIGHT}, panelParams);

	struct SDLGUI_Element *canvas = SDLGUI_Create_Virtual(
		(SDL_Rect){.x=GUI_LEFTPANEL_WIDTH, .y=GUI_TOPBAR_HEIGHT, .w=SDLGUI_SIZE_FILL, .h=SDLGUI_SIZE_FILL},
		&drawCanvas, &handleMouseButton, &handleMouseMotion, &handleResize
	);

	struct SDLGUI_List *list = SDLGUI_List_Create( 3);

	SDLGUI_List_Add( list, leftPanel);
	SDLGUI_List_Add( list, topBar);
	SDLGUI_List_Add( list, canvas);

	SDLGUI_Layer_Add( list);



	SDLGUI_AddTo_Panel( leftPanel, SDLGUI_Create_Text( (SDL_Rect){.x=0, .y=10, .w=GUI_LEFTPANEL_WIDTH}, " YZ-01 \n-------", (SDLGUI_Params){
			.bgColor=labelParams.bgColor,
			.fgColor=labelParams.fgColor,
			.fontWidth=18,
			.fontHeight=24
		}
	));

	struct SDLGUI_Element *saveButton = SDLGUI_Create_Text( (SDL_Rect){.x=10, .y=60, .w=GUI_LEFTPANEL_WIDTH/2 - 15, .h=20}, "Save", buttonParams);
	struct SDLGUI_Element *quitButton = SDLGUI_Create_Text( (SDL_Rect){.x=GUI_LEFTPANEL_WIDTH/2 + 5, .y=60, .w=GUI_LEFTPANEL_WIDTH/2 - 15, .h=20}, "Quit", buttonParams);
	SDLGUI_AddTo_Panel( leftPanel, saveButton);
	SDLGUI_AddTo_Panel( leftPanel, quitButton);
	saveButton->clicked = buttonSave_clicked;
	quitButton->clicked = buttonQuit_clicked;

	const int buttonsPerRow=3;
	const int buttonSize = 32;
	const int iconSize = 16;
	const int buttonLeftMargin = (GUI_LEFTPANEL_WIDTH - 2*10 - buttonsPerRow*buttonSize ) / (buttonsPerRow+1);
	const int buttonSizeWithMargins = buttonSize + buttonLeftMargin;

	struct buttonTemplate {
		char *fileName;
		SDLGUI_Clicked *onClick;
	};
	struct buttonTemplate buttonTemplates[] = {
		{"res/editor/move.png",         button_move_clicked},
		{"res/editor/rotate.png",       button_rotate_clicked},
		{"res/editor/resize.png",       button_resize_clicked},
		{"res/editor/terrain.png",      button_terrain_clicked},
		{"res/editor/object.png",       button_object_clicked},
		{"res/editor/ai.png",           button_ai_clicked},
		{"res/editor/delete.png",       button_remove_clicked},
		{"res/editor/template.png",     button_template_clicked},
		{"res/editor/object.png",       button_texture_clicked},
	};

	struct SDLGUI_Element *buttonsContainer = SDLGUI_Create_Panel( (SDL_Rect){.x=10, .y=100, .w=GUI_LEFTPANEL_WIDTH-2*10, .h= buttonLeftMargin + ((sizeof(buttonTemplates) / sizeof(struct buttonTemplate) - 1) / buttonsPerRow + 1) * buttonSizeWithMargins}, panelParams);
	SDLGUI_AddTo_Panel( leftPanel, buttonsContainer);

	int i;
	for( i=0; i<sizeof(buttonTemplates) / sizeof(struct buttonTemplate); i++) {
		struct buttonTemplate *template = &buttonTemplates[i];

		SDL_Texture *texture = loadTexture( renderer, template->fileName);

		struct SDLGUI_Element *element = SDLGUI_Create_Texture( (SDL_Rect){.x=buttonLeftMargin + (i%buttonsPerRow) *buttonSizeWithMargins, .y= buttonLeftMargin + (i/buttonsPerRow) *buttonSizeWithMargins, .w=buttonSize, .h=buttonSize}, texture, iconSize, iconSize, buttonParams);

		SDL_DestroyTexture( texture);

		element->clicked = template->onClick;
		SDLGUI_AddTo_Panel( buttonsContainer, element);
	}

	struct SDLGUI_Element *brushOptionsContainer = SDLGUI_Create_Panel( (SDL_Rect){.x=buttonsContainer->rect.x, .y=300, .w=buttonsContainer->rect.w, .h=250}, (SDLGUI_Params) {
			.bgColor = COLOR_TRANSPARENT,
			.borderThickness = 0
		}
	);
	SDLGUI_AddTo_Panel( leftPanel, brushOptionsContainer);

	brushOptionPanels_init( brushOptionsContainer, textures->obj, textures->trn, &selectedObjStuff.obj, renderer);


	selectedObjStuff.panel = SDLGUI_Create_Panel( (SDL_Rect){.x=buttonsContainer->rect.x, .y=700, .w=buttonsContainer->rect.w, .h=200}, panelParams);
	selectedObjStuff.panel->isVisible = false;
	SDLGUI_AddTo_Panel( leftPanel, selectedObjStuff.panel);


	SDLGUI_AddTo_Panel( selectedObjStuff.panel, SDLGUI_Create_Text( (SDL_Rect){.x=0, .y=0, .w=GUI_LEFTPANEL_WIDTH - 20, .h=20}, "Selected Obj", (SDLGUI_Params){
		.fontWidth = 12,
		.fontHeight = 16,
		.bgColor = COLOR_TRANSPARENT,
		.fgColor = COLOR_BLACK
	}));

	SDLGUI_AddTo_Panel( topBar, SDLGUI_Create_Text( (SDL_Rect){.x= 150, .y=0, .w=TEXT_SPAN_SIZE, .h=GUI_TOPBAR_HEIGHT}, "TODO: map name", labelParams));


	struct itemTemplate {
		char *label;
		struct SDLGUI_Element **elementPtr;
		int numRows;
		bool isReadOnly;
		SDLGUI_TextBox_Changed *textChangeCallback;
		SDLGUI_Checkbox_Changed *checkboxChangedCallback;
	};
	struct itemTemplate itemsToCreate[] = {
		{ "          id", 	&selectedObjStuff.textboxes.id			, 1, false,	textbox_id_changed 			, NULL 	},
		{ "      health", 	&selectedObjStuff.textboxes.health		, 1, false,	textbox_health_changed 		, NULL 	},
		{ "  max health", 	&selectedObjStuff.textboxes.maxHealth 	, 1, false,	textbox_maxHealth_changed 	, NULL 	},
		{ "  is movable", 	&selectedObjStuff.checkboxes.movable	, 1, false,	NULL						, checkbox_movable_changed 	},
		{ " is pickable", 	&selectedObjStuff.checkboxes.pickable	, 1, false,	NULL						, checkbox_pickable_changed },
		{ "         ATK", 	&selectedObjStuff.textboxes.attack		, 1, false,	textbox_attack_changed 		, NULL 	},
		{ "         DEF", 	&selectedObjStuff.textboxes.defence		, 1, false,	textbox_defence_changed 	, NULL 	},
		{ "          AI", 	&selectedObjStuff.textboxes.ai			, 3, true,	NULL 						, NULL 	},
	};
	SDLGUI_Params textboxParams = labelParams;
	textboxParams.bgColor = COLOR_WHITE;
	textboxParams.borderThickness = 1;


	const int textboxPairHeight = 15;
	for( i=0; i<sizeof(itemsToCreate) / sizeof(struct itemTemplate); i++) {
		struct itemTemplate *template = &itemsToCreate[i];
		int yVal = 25+i*textboxPairHeight;

		struct SDLGUI_Element *label = SDLGUI_Create_Text( (SDL_Rect){.x=10, .y=yVal, .w=80, .h=textboxPairHeight * template->numRows}, template->label, labelParams);
		struct SDLGUI_Element *value;

		if ( template->checkboxChangedCallback == NULL) {
			value = SDLGUI_Create_Textbox( (SDL_Rect){.x=90, .y=yVal, .w=80, .h=textboxPairHeight * template->numRows}, textboxParams);
			if( template->isReadOnly != true)
				value->data.textData->acceptedChars  = TEXTBOX_INPUT_NUMERIC;
			value->data.textData->textChanged = template->textChangeCallback;
		}
		else {
			value = SDLGUI_Create_Checkbox( (SDL_Rect){.x=90, .y=yVal, .w=0, .h=textboxPairHeight * template->numRows}, textboxParams);
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

void mapLoaded() {
	if(spriteSpecs == NULL) {
		char *schedulePath = calculateSpriteSpecsFilePath( myMap->filePath);
		spriteSpecs = readSpriteSpecsFile (schedulePath);
		free( schedulePath);

		drawBackground();
		draw();

		if(spriteSpecs) {
			loadObjectTextures( renderer, textures, spriteSpecs);
		}

		reloadTextureButtons();
	}
}

int main( int argc, char *args[]) {
	//Default values
	myMap = 0;
	brush.fun = editor_move;
	brush.isRepeat =  false;
	templates_load();

	init( GUI_LEFTPANEL_WIDTH, GUI_TOPBAR_HEIGHT, 1280, 960);
	log0("loading textures\n");
	textures = loadOrdinaryTextures( renderer);
	SDLGUI_Init( window, renderer, textures->font);
	initGui();

	if( argc > 1) {
		myMap = readMapFile( args[1]);

		if( myMap->objListCount > 0) {
			objectCounter = myMap->objList[ myMap->objListCount-1 ]->id + 1;
		}

		mapLoaded();
	}
	else {
		createMapCreator(&mapLoaded);
	}

	running = true;
	run();

	templates_save();

	log0("Program over\nPeace\n");

	SDLGUI_Destroy();
	if (myMap)
		freeMap( myMap);
	freeTextures( textures);
	if (spriteSpecs)
		destroySpriteSpecs (spriteSpecs);
	SDL_DestroyRenderer( renderer);
	SDL_DestroyWindow( window);
	SDL_Quit();
	return 0;
}
