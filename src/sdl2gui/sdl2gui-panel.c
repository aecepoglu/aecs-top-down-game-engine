#include <stdlib.h>
#include "sdl2gui.h"
#include "sdl2gui-panel.h"
#include "sdl2gui-list.h"

struct SDLGUI_List* SDLGUI_Get_Panel_Elements( struct SDLGUI_Element *e) {
	return e->data.elements;
}

void SDLGUI_Draw_Panel( struct SDLGUI_Element *element) {
	if( ! element->isVisible) 
		return;

	SDL_RenderCopy( guiCore.renderer, element->textures.normal, NULL, &element->rect);

	struct SDLGUI_List *elements = SDLGUI_Get_Panel_Elements(element);
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

void SDLGUI_ResizePanel(struct SDLGUI_Element *panel, int width, int height) {
	if (panel->sizeHints & SDLGUI_SIZEHINTS_STRETCH_HORIZONTAL) {
		panel->rect.w = width - panel->rect.x;
	}
	else if (panel->sizeHints & SDLGUI_SIZEHINTS_STRETCH_VERTICAL) {
		panel->rect.h = height - panel->rect.y;
	}
	else {
		return;
	}
}

struct SDLGUI_Element* SDLGUI_Create_Panel( SDL_Rect rect, SDLGUI_Params params) {
	struct SDLGUI_Element *panel = SDLGUI_CreateElement(rect);

    panel->textures.current = panel->textures.normal = createElementTexture( panel->rect.w, panel->rect.h, params.bgColor, params.fgColor, params.borderThickness, 0, 0, 0, 0);
	panel->textures.hover = panel->textures.focused = NULL;
    panel->data.elements = SDLGUI_List_Create( 4);
	panel->destructor = SDLGUI_Destroy_Panel;
	panel->drawFun = SDLGUI_Draw_Panel;
	panel->mouseHandler = SDLGUI_MouseHandler_Panel;
	panel->isVisible = !params.isHidden;
	panel->resizeHandler = &SDLGUI_ResizePanel;

	return panel;
}

void SDLGUI_AddTo_Panel( struct SDLGUI_Element *panel, struct SDLGUI_Element *element) {
	SDLGUI_List_Add( panel->data.elements, element);
	element->rect.x += panel->rect.x;
	element->rect.y += panel->rect.y;
}

void SDLGUI_Clear_Panel( struct SDLGUI_Element *p) {
	SDLGUI_List_Clear( p->data.elements, 1);
}
