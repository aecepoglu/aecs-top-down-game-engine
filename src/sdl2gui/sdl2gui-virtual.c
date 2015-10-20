#include <stdlib.h>
#include "sdl2gui.h"
#include "sdl2gui-virtual.h"

void SDLGUI_Draw_Virtual(struct SDLGUI_Element *e) {
	e->data.virtualData->onDraw(&e->rect);
}

void SDLGUI_Destroy_Virtual( struct SDLGUI_Element *e) {
	free(e->data.virtualData);
	free(e);
};

void SDLGUI_ResizeVirtual( struct SDLGUI_Element *e, int width, int height ) {
	if ( e->sizeHints & SDLGUI_SIZEHINTS_STRETCH_HORIZONTAL ) {
		e->rect.w = width - e->rect.x;
	}
	if ( e->sizeHints & SDLGUI_SIZEHINTS_STRETCH_VERTICAL ) {
		e->rect.h = height - e->rect.y;
	}
	if (( e->sizeHints & !SDLGUI_SIZEHINTS_FIXED )
		&& (e->data.virtualData->onResize )
	) {
		e->data.virtualData->onResize(&e->rect);
	}
}

struct SDLGUI_Element* SDLGUI_Create_Virtual(SDL_Rect rect, SDLGUI_DrawCallback *onDraw, SDLGUI_MouseDownFunction *mouseDown, SDLGUI_MouseMotionFunction  *mouseMotion, SDLGUI_ResizeCallback *onResize ) {
	struct SDLGUI_Virtual_Data *data = (struct SDLGUI_Virtual_Data*)malloc( sizeof( struct SDLGUI_Virtual_Data));
	data->onDraw = onDraw;
	data->onResize = onResize;

	struct SDLGUI_Element *element = SDLGUI_CreateElement(rect);

	element->destructor = SDLGUI_Destroy_Virtual;
	element->drawFun = SDLGUI_Draw_Virtual;
	element->mouseDown = mouseDown;
	element->mouseMotion = mouseMotion;
	element->isVisible = 1;
	element->data.virtualData = data;
	element->resizeHandler = SDLGUI_ResizeVirtual;

	return element;
}
