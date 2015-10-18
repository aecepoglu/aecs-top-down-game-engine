#ifndef SDLGUI_VIRTUAL_H
#define SDLGUI_VIRTUAL_H

#include "sdl2gui-element.h"

struct SDLGUI_Element* SDLGUI_Create_Virtual(SDL_Rect rect, void (*drawCallback)(), SDLGUI_MouseDownFunction *mouseDown, SDLGUI_MouseMotionFunction  *mouseMotion );

#endif /*SDLGUI_VIRTUAL_H*/
