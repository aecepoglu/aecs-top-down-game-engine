#include <stdlib.h>
#include "sdl2gui-virtual.h"

struct SDLGUI_Virtual_Data {
	void (*drawCallback)();
};

void SDLGUI_Draw_Virtual(struct SDLGUI_Element *e) {
	( (struct SDLGUI_Virtual_Data*)e->userData )->drawCallback(&e->rect);
}

void SDLGUI_Destroy_Virtual( struct SDLGUI_Element *e) {
	free((struct SDLGUI_Virtual_Data*)e->userData);
	free(e);
};

struct SDLGUI_Element* SDLGUI_Create_Virtual(SDL_Rect rect, void (*drawCallback)() ) {
	struct SDLGUI_Virtual_Data *data = (struct SDLGUI_Virtual_Data*)malloc( sizeof( struct SDLGUI_Virtual_Data));
	data->drawCallback = drawCallback;
	
	struct SDLGUI_Element *element = (struct SDLGUI_Element*)malloc(sizeof(struct SDLGUI_Element));
	
	element->rect = rect;
	element->clicked = 0;
	element->destructor = SDLGUI_Destroy_Virtual;
	element->drawFun = SDLGUI_Draw_Virtual;
	element->mouseHandler = 0;
	element->isVisible = 1;
	element->userData = data;

	return element;
}
