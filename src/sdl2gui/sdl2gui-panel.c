#include <stdlib.h>
#include "sdl2gui.h"
#include "sdl2gui-panel.h"
#include "sdl2gui-list.h"

void SDLGUI_Draw_Panel( struct SDLGUI_Element *element) {
	if( ! element->isVisible) 
		return;

	SDL_RenderCopy( guiCore.renderer, element->textures.normal, NULL, &element->rect);

	struct SDLGUI_List *elements = element->data.elements;
    int i;
    for( i=0; i<elements->count; i++) {
        struct SDLGUI_Element *e = elements->list[i];
        if( e->drawFun != 0)
            e->drawFun( e);
    }
}

void SDLGUI_Destroy_Panel( struct SDLGUI_Element *element) {
    SDLGUI_List_Destroy( element->data.elements);
    SDL_DestroyTexture( element->textures.normal);
    free( element);
}

struct SDLGUI_Element* SDLGUI_MouseHandler_Panel( struct SDLGUI_Element *panel, int x, int y) {
	struct SDLGUI_Element *result = SDLGUI_List_findItemAtPos( panel->data.elements, x, y);
	return result != 0 ? result : panel;
}

struct SDLGUI_Element* SDLGUI_Create_Panel( SDL_Rect rect, SDLGUI_Params params) {
	struct SDLGUI_Element *panel = (struct SDLGUI_Element*)malloc( sizeof( struct SDLGUI_Element));

	if( rect.w == SDLGUI_SIZE_FILL || rect.h == SDLGUI_SIZE_FILL) {
		int winWidth, winHeight;

		SDL_GetWindowSize( guiCore.window, &winWidth, &winHeight);

		if( rect.w == SDLGUI_SIZE_FILL)
			rect.w = winWidth - rect.x;

		if( rect.h == SDLGUI_SIZE_FILL)
			rect.h = winHeight - rect.y;
	}
    
    panel->textures.current = panel->textures.normal = createElementTexture( rect.w, rect.h, params.bgColor, params.fgColor, params.borderThickness, 0, 0, 0);
	panel->textures.hover = panel->textures.focused = NULL;
    panel->data.elements = SDLGUI_List_Create( 4);
	panel->rect = rect;
	panel->clicked = NULL;
	panel->destructor = SDLGUI_Destroy_Panel;
	panel->drawFun = SDLGUI_Draw_Panel;
	panel->mouseHandler = SDLGUI_MouseHandler_Panel;
	panel->textInputHandler = NULL;
	panel->isVisible = !params.isHidden;

	return panel;
}

void SDLGUI_AddTo_Panel( struct SDLGUI_Element *panel, struct SDLGUI_Element *element) {
	SDLGUI_List_Add( panel->data.elements, element);
	element->rect.x += panel->rect.x;
	element->rect.y += panel->rect.y;
}
