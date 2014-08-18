#include <stdlib.h>

#include "sdl2gui.h"
#include "../text.h"
#include "../log.h"


struct SDLGUI_Core {
	SDL_Renderer *renderer;
	SDL_Texture **font;

	struct SDLGUI_List elements;

	struct SDLGUI_Element *mouseDownElement;
} guiCore;

/* ---------------------
	SDLGUI Core
*/

#define IS_POINT_IN_RECT( p, r) 

void SDLGUI_Init( SDL_Renderer *renderer, SDL_Texture **font) {
	guiCore.renderer = renderer;
	guiCore.font = font;

	SDLGUI_List_Init( &guiCore.elements, 4);
}

void SDLGUI_Destroy() {
	SDLGUI_List_Destroy( &guiCore.elements, 0);
	guiCore.renderer = 0;
	guiCore.font = 0;
}

void SDLGUI_Draw() {
	int i;
	for( i=0; i<guiCore.elements.count; i++) {
		guiCore.elements.list[i]->drawFun( guiCore.elements.list[i], 0, 0);
	}

	log3("sdlgui_draw()\n");
}

struct SDLGUI_Element* SDLGUI_Handle_MouseDown_List( struct SDLGUI_List *list, SDL_MouseButtonEvent *e) {
	struct SDLGUI_Element *elem;
	int i;
	for(i=0; i<list->count; i++) {
		elem = list->list[i];
		if (e->x >= elem->rect.x && e->y >= elem->rect.y && e->x<elem->rect.x+elem->rect.w && e->y<elem->rect.y+elem->rect.h) {
			if( elem->mouseDownHandler != 0)
				return elem->mouseDownHandler( elem, e);
			else
				return elem;
		}
	}
	return 0;
}

int SDLGUI_Handle_MouseDown( SDL_MouseButtonEvent *e) {
	struct SDLGUI_Element *mouseDownElement = SDLGUI_Handle_MouseDown_List( &guiCore.elements, e);
	if( mouseDownElement != 0) {
		guiCore.mouseDownElement = mouseDownElement;
		return 1;
	}
	else
		return 0;
}

void SDLGUI_Handle_MouseUp( SDL_MouseButtonEvent *e) {
	if( guiCore.mouseDownElement->clicked != 0)
		guiCore.mouseDownElement->clicked( guiCore.mouseDownElement);
	guiCore.mouseDownElement = 0;
}

void SDLGUI_Add_Element( struct SDLGUI_Element *element) {
	log0("add element %p \n", element);
	SDLGUI_List_Add( &guiCore.elements, element);
	log0("added element to %p\n", guiCore.elements.list[ guiCore.elements.count - 1]);
}

void SDLGUI_Remove_Element( struct SDLGUI_Element *element) {
	SDLGUI_List_Remove( &guiCore.elements, element);
}


/* ------------------
	SDLGUI Elements
*/

SDL_Texture *createElementTexture( int width, int height, int bgColor[4], int borderColor[4], int borderThickness, SDL_Texture *fgTexture, int fgWidth, int fgHeight) {
	SDL_Texture *texture = SDL_CreateTexture( guiCore.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height) ;
	SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND);


	SDL_SetRenderTarget( guiCore.renderer, texture);

	SDL_SetRenderDrawColor( guiCore.renderer, borderColor[0], borderColor[1], borderColor[2], borderColor[3]);
	SDL_RenderClear( guiCore.renderer);

	SDL_SetRenderDrawColor( guiCore.renderer, bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	SDL_Rect rect = {.x=borderThickness, .y=borderThickness, .w=width-2*borderThickness, .h=height-2*borderThickness};
	SDL_RenderFillRect( guiCore.renderer, &rect);

    if( fgTexture) {
	    //TODO assert width > fgWidth, height>fgHeight
	    rect.x = (width - fgWidth) / 2;
	    rect.y = (height - fgHeight) / 2;
	    rect.w = fgWidth;
	    rect.h = fgHeight;
	    SDL_RenderCopy( guiCore.renderer, fgTexture, NULL, &rect);
    }

	SDL_SetRenderTarget( guiCore.renderer, 0);

	return texture;
}

struct SDLGUI_Element* SDLGUI_Create_Element( int xPos, int yPos, int width, int height, void *data, SDLGUI_Clicked *clicked, SDLGUI_Destructor *dtor, SDLGUI_DrawFun *drawFun, SDLGUI_MouseDownHandler *mouseDownHandler, void *userData) {
	struct SDLGUI_Element *e = (struct SDLGUI_Element*) malloc( sizeof( struct SDLGUI_Element));

	e->rect.x = xPos;
	e->rect.y = yPos;
	e->rect.w = width;
	e->rect.h = height;
	e->data = data;

	e->clicked = clicked;
	e->destructor = dtor;
	e->drawFun = drawFun;
	e->mouseDownHandler = mouseDownHandler;
	e->userData = userData;

	log0("created element %p\n", e);

	return e;
}

/* Texture
*/

void SDLGUI_Draw_Texture( struct SDLGUI_Element *element, int offsetX, int offsetY){
	SDL_Rect rect = {element->rect.x, element->rect.y, element->rect.w, element->rect.h};
	rect.x += offsetX;
	rect.y += offsetY;
	SDL_RenderCopy( guiCore.renderer, (SDL_Texture*)element->data, NULL, &rect);
}

struct SDLGUI_Element* SDLGUI_Create_Texture( int xPos, int yPos, int width, int height, SDL_Texture *texture, SDLGUI_Clicked *clicked, void *userData) {
	return SDLGUI_Create_Element( xPos, yPos, width, height, texture, clicked, &SDLGUI_Destroy_Texture, &SDLGUI_Draw_Texture, 0, userData);
}

void SDLGUI_Destroy_Texture( struct SDLGUI_Element *element) {
	SDL_Texture *t = (SDL_Texture*)element->data;
	SDL_DestroyTexture( t);
	free( element);
	log0("destroyed texture\n");
}

/* Text
*/

struct SDLGUI_Element* SDLGUI_Create_Text( int xPos, int yPos, int width, int height, SDLGUI_Clicked *clicked, const char *text, int bgColor[4], int fgColor[4], int fontWidth, int fontHeight, int borderThickness, void *userData) {
	
	int textWidth, textHeight;
	SDL_Texture *textTexture = getTextTexture( guiCore.renderer, guiCore.font, text, fontWidth, fontHeight, (int[4]){0,0,0,0}, fgColor[0], fgColor[1], fgColor[2], &textWidth, &textHeight);

	SDL_Texture *elementTexture = createElementTexture( width, height, bgColor, fgColor, borderThickness, textTexture, textWidth, textHeight);
	SDL_DestroyTexture( textTexture);

	return SDLGUI_Create_Texture( xPos, yPos, width, height, elementTexture, clicked, userData);
}

/* Panel
*/

void SDLGUI_Draw_Panel( struct SDLGUI_Element *element, int offsetX, int offsetY) {
    struct SDLGUI_Panel_Data *data = (struct SDLGUI_Panel_Data*)element->data;

	SDL_Rect rect = {element->rect.x + offsetX, element->rect.y + offsetY, element->rect.w, element->rect.h};
	
    SDL_RenderCopy( guiCore.renderer, data->texture, 0, &rect);

    int i;
    for( i=0; i<data->elements->count; i++) {
        struct SDLGUI_Element *e = data->elements->list[i];
        if( e->drawFun != 0)
            e->drawFun( e, element->rect.x, element->rect.y);
    }
}

void SDLGUI_Destroy_Panel( struct SDLGUI_Element *element) {
    struct SDLGUI_Panel_Data *data = (struct SDLGUI_Panel_Data*)element->data;

    SDLGUI_List_Destroy( data->elements, 1);
    SDL_DestroyTexture( data->texture);
    free( data);
    free( element);
}

struct SDLGUI_Element* SDLGUI_MouseDownHandler_Panel( struct SDLGUI_Element *panel, SDL_MouseButtonEvent *e) {
	e->x -= panel->rect.x;
	e->y -= panel->rect.y;
	struct SDLGUI_Panel_Data *data = (struct SDLGUI_Panel_Data*)panel->data;
	struct SDLGUI_Element *result = SDLGUI_Handle_MouseDown_List( data->elements, e);
	return result != 0 ? result : panel;
}

struct SDLGUI_Element* SDLGUI_Create_Panel( int xPos, int yPos, int width, int height, int bgColor[4], int borderColor[4], int borderThickness) {
    
    struct SDLGUI_Panel_Data *panelData = (struct SDLGUI_Panel_Data*)malloc( sizeof( struct SDLGUI_Panel_Data));
    panelData->texture = createElementTexture( width, height, bgColor, borderColor, borderThickness, 0, 0, 0);
    panelData->elements = SDLGUI_List_Create( 4);

    return SDLGUI_Create_Element( xPos, yPos, width, height, panelData, 0, &SDLGUI_Destroy_Panel, &SDLGUI_Draw_Panel, &SDLGUI_MouseDownHandler_Panel, 0);
}
