#include <stdlib.h>
#include "sdl2gui.h"
#include "sdl2gui-panel.h"
#include "sdl2gui-list.h"

#define SCROLLER_WIDTH 3

struct SDLGUI_List* SDLGUI_Get_Panel_Elements( struct SDLGUI_Element *e) {
	return e->data.panel.elements;
}

void SDLGUI_Draw_Panel( struct SDLGUI_Element *element) {
	if( ! element->isVisible) 
		return;

	SDL_RenderCopy( guiCore.renderer, element->textures.normal, NULL, &element->rect);

	int panelYMax = element->rect.y + element->rect.h;

	struct SDLGUI_List *elements = SDLGUI_Get_Panel_Elements(element);
	int i;
	for (i=0; i<elements->count; i++) {
		struct SDLGUI_Element *e = elements->list[i];
		if (e->drawFun != 0 &&
		   (e->rect.y + e->rect.h) <= panelYMax &&
		   e->rect.y >= element->rect.y ) {
			e->drawFun(e);
		}
	}

	if (element->data.panel.contentHeight > element->rect.h) {
		float scrollRatio = (float)element->rect.h / (float)element->data.panel.contentHeight;

		int scrollerHeight = element->rect.h * scrollRatio;
		int scrollerPosY = element->data.panel.scrollY * scrollRatio;

		SDL_SetRenderDrawColor( guiCore.renderer, 0, 0, 0, 255);

		SDL_RenderFillRect(guiCore.renderer, &(SDL_Rect){
			.x = element->rect.x + element->rect.w - 2 - SCROLLER_WIDTH,
			.y = scrollerPosY + element->rect.y,
			.w = SCROLLER_WIDTH,
			.h = scrollerHeight
		});
	}
}

void SDLGUI_Destroy_Panel( struct SDLGUI_Element *element) {
	SDLGUI_List_Destroy( element->data.panel.elements);
	SDL_DestroyTexture( element->textures.normal);
	free( element);
}

struct SDLGUI_Element* SDLGUI_MouseHandler_Panel( struct SDLGUI_Element *panel, int x, int y) {
	struct SDLGUI_Element *result = SDLGUI_List_findItemAtPos( panel->data.panel.elements, x, y);

	if (result != 0)
		return result;
	else
		return panel;
}

void SDLGUI_ResizePanel(struct SDLGUI_Element *panel, int width, int height) {
	if (panel->sizeHints & SDLGUI_SIZEHINTS_STRETCH_HORIZONTAL) {
		panel->rect.w = width - panel->rect.x;
	}
	else if (panel->sizeHints & SDLGUI_SIZEHINTS_STRETCH_VERTICAL) {
		panel->rect.h = height - panel->rect.y;
	}
}

int SDLGUI_ScrollPanel(struct SDLGUI_Element *panel, int dy) {
	int targetScrollY = panel->data.panel.scrollY + dy;

	if (targetScrollY + panel->rect.h > panel->data.panel.contentHeight &&
	    panel->data.panel.scrollY + panel->rect.h < panel->data.panel.contentHeight) {

		targetScrollY = panel->data.panel.contentHeight - panel->rect.h;
	}
	else if (targetScrollY < 0 &&
	         panel->data.panel.scrollY > 0) {

		targetScrollY = 0;
	}

	if (targetScrollY + panel->rect.h <= panel->data.panel.contentHeight &&
	    targetScrollY >= 0) {

		panel->data.panel.scrollY = targetScrollY;

		struct SDLGUI_List *elements = SDLGUI_Get_Panel_Elements(panel);

		int i;
		for (i=0; i<elements->count; i++) {
			struct SDLGUI_Element *e = elements->list[i];

			e->rect.y -= dy;
		}

		return 1;
	}
	else {
		return 0;
	}
}

struct SDLGUI_Element* SDLGUI_Create_Panel( SDL_Rect rect, SDLGUI_Params params) {
	struct SDLGUI_Element *panel = SDLGUI_CreateElement(rect);

	panel->textures.current = panel->textures.normal = createElementTexture( panel->rect.w, panel->rect.h, params.bgColor, params.fgColor, params.borderThickness, 0, 0, 0, 0);
	panel->textures.hover = panel->textures.focused = NULL;
	panel->data.panel.elements = SDLGUI_List_Create( 4);
	panel->data.panel.scrollY = 0;
	panel->data.panel.contentHeight = 0;
	panel->destructor = SDLGUI_Destroy_Panel;
	panel->drawFun = SDLGUI_Draw_Panel;
	panel->mouseHandler = SDLGUI_MouseHandler_Panel;
	panel->isVisible = !params.isHidden;
	panel->resizeHandler = &SDLGUI_ResizePanel;
	panel->scrollHandler = &SDLGUI_ScrollPanel;

	return panel;
}

void SDLGUI_AddTo_Panel( struct SDLGUI_Element *panel, struct SDLGUI_Element *element) {
	int elementYMax = element->rect.y + element->rect.h;

	if (elementYMax > panel->data.panel.contentHeight) {
		panel->data.panel.contentHeight = elementYMax;
	}

	SDLGUI_List_Add( panel->data.panel.elements, element);
	element->rect.x += panel->rect.x;
	element->rect.y += panel->rect.y;
	element->parent = panel;
}

void SDLGUI_Clear_Panel( struct SDLGUI_Element *p) {
	SDLGUI_List_Clear( p->data.panel.elements, 1);

	p->data.panel.contentHeight = 0;
	p->data.panel.scrollY = 0;
}
