#ifndef SDLGUI_VIRTUAL_H
#define SDLGUI_VIRTUAL_H

#include "sdl2gui-element.h"

typedef void (SDLGUI_ResizeCallback)(const SDL_Rect *rect);
typedef void (SDLGUI_DrawCallback)();

struct SDLGUI_Virtual_Data {
	SDLGUI_DrawCallback *onDraw;
	SDLGUI_ResizeCallback *onResize;
};

struct SDLGUI_Element* SDLGUI_Create_Virtual(SDL_Rect rect, SDLGUI_DrawCallback *onDraw, SDLGUI_MouseDownFunction *mouseDown, SDLGUI_MouseMotionFunction  *mouseMotion, SDLGUI_ResizeCallback *onResize );

#endif /*SDLGUI_VIRTUAL_H*/
