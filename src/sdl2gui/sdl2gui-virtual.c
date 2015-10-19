#include <stdlib.h>
#include "sdl2gui.h"
#include "sdl2gui-virtual.h"

void SDLGUI_Draw_Virtual(struct SDLGUI_Element *e) {
	e->data.virtualData->drawCallback(&e->rect);
}

void SDLGUI_Destroy_Virtual( struct SDLGUI_Element *e) {
	free(e->data.virtualData);
	free(e);
};

struct SDLGUI_Element* SDLGUI_Create_Virtual(SDL_Rect rect, void (*drawCallback)(), SDLGUI_MouseDownFunction *mouseDown, SDLGUI_MouseMotionFunction  *mouseMotion ) {
	struct SDLGUI_Virtual_Data *data = (struct SDLGUI_Virtual_Data*)malloc( sizeof( struct SDLGUI_Virtual_Data));
	data->drawCallback = drawCallback;

	struct SDLGUI_Element *element = SDLGUI_CreateElement();

	if( rect.w == SDLGUI_SIZE_FILL || rect.h == SDLGUI_SIZE_FILL) {
		int winWidth, winHeight;

		SDL_GetWindowSize( guiCore.window, &winWidth, &winHeight);

		if( rect.w == SDLGUI_SIZE_FILL)
			rect.w = winWidth - rect.x;

		if( rect.h == SDLGUI_SIZE_FILL)
			rect.h = winHeight - rect.y;
	}

	element->rect = rect;
	element->clicked = 0;
	element->destructor = SDLGUI_Destroy_Virtual;
	element->drawFun = SDLGUI_Draw_Virtual;
	element->mouseHandler = 0;
	element->mouseDown = mouseDown;
	element->mouseMotion = mouseMotion;
	element->isVisible = 1;
	element->data.virtualData = data;

	return element;
}
