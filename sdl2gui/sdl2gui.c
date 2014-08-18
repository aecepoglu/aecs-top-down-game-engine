#include "sdl2gui.h"

#include "../text.h"
#include "stdlib.h"
#include "sdl2gui-list.h"

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
		guiCore.elements.list[i]->drawFun( guiCore.elements.list[i]);
	}

	log3("sdlgui_draw()\n");
}

int SDLGUI_Handle_MouseDown( SDL_MouseButtonEvent *e) {
	struct SDLGUI_Element *elem;
	int i;
	for(i=0; i<guiCore.elements.count; i++) {
		elem = guiCore.elements.list[i];
		if (e->x >= elem->rect.x && e->y >= elem->rect.y && e->x<elem->rect.x+elem->rect.w && e->y<elem->rect.y+elem->rect.h) {
			guiCore.mouseDownElement = elem;
			return 1;
		}
	}
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

void SDLGUI_Draw_Texture( struct SDLGUI_Element *element){
	SDL_RenderCopy( guiCore.renderer, (SDL_Texture*)element->data, NULL, &element->rect);
}

void SDLGUI_Destroy_Texture( struct SDLGUI_Element *element) {
	SDL_Texture *t = (SDL_Texture*)element->data;
	SDL_DestroyTexture( t);
	free( element);
	log0("destroyed texture\n");
}

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

struct SDLGUI_Element* SDLGUI_Create_Element( int xPos, int yPos, int width, int height, void *data, SDLGUI_Clicked *clicked, SDLGUI_Destructor *dtor, SDLGUI_DrawFun *drawFun) {
	struct SDLGUI_Element *e = (struct SDLGUI_Element*) malloc( sizeof( struct SDLGUI_Element));

	e->rect.x = xPos;
	e->rect.y = yPos;
	e->rect.w = width;
	e->rect.h = height;
	e->data = data;

	e->clicked = clicked;
	e->destructor = dtor;
	e->drawFun = drawFun;

	log0("created element %p\n", e);

	return e;
}

struct SDLGUI_Element* SDLGUI_Create_Text( int xPos, int yPos, int width, int height, SDLGUI_Clicked *clicked, const char *text, int bgColor[4], int fgColor[4], int fontWidth, int fontHeight) {
	
	int textWidth, textHeight;
	SDL_Texture *textTexture = getTextTexture( guiCore.renderer, guiCore.font, text, fontWidth, fontHeight, (int[4]){0,0,0,0}, fgColor[0], fgColor[1], fgColor[2], &textWidth, &textHeight);

	SDL_Texture *elementTexture = createElementTexture( width, height, bgColor, fgColor, 2, textTexture, textWidth, textHeight);
	SDL_DestroyTexture( textTexture);

	return SDLGUI_Create_Texture( xPos, yPos, width, height, elementTexture, clicked);
}

/* Panel
*/

void SDLGUI_Draw_Panel( struct SDLGUI_Element *element) {
    struct SDLGUI_Panel_Data *data = (struct SDLGUI_Panel_Data*)element->data;
	
    SDL_RenderCopy( guiCore.renderer, data->texture, 0, &element->rect);

    int i;
    for( i=0; i<data->elements.count; i++) {
        struct SDLGUI_Element *e = data->elements[i];
        if( e->drawFun != 0)
            e->drawFun( e);
    }
}

void SDLGUI_Destroy_Panel( struct SDLGUI_Element *element) {
    struct SDLGUI_Panel_Data *data = (struct SDLGUI_Panel_Data*)element->data;

    SDLGUI_List_Destroy( &data->elements);
    SDL_DestroyTexture( data->texture);
    free( data);
    free( element);
}

struct SDLGUI_Element* SDLGUI_Create_Panel( int xPos, int yPos, int width, int height, int bgColor[4], int borderColor[4], int borderWidth) {
    
    struct SDLGUI_Panel_Data *panelData = (struct SDLGUI_Panel_Data*)malloc( sizeof( struct SDLGUI_Panel_Data));
    panelData->texture = createElementTexture( width, height, bgColor[4], borderColor[4], borderThickness, 0, 0, 0);
    SDLGUI_List_Init( &panelData->elements, 4);

    return SDLGUI_Create_Element( xPos, yPos, width, height, panelData, 0, SDLGUI_Destroy_Panel, SDLGUI_Draw_Panel);
}
