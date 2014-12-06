#ifndef SDLGUI_PANEL_H
#define SDLGUI_PANEL_H

#include "sdl2gui-element.h"

void SDLGUI_Destroy_Panel( struct SDLGUI_Element *element);

struct SDLGUI_Element* SDLGUI_Create_Panel( SDL_Rect rect, SDLGUI_Params params);

void SDLGUI_AddTo_Panel( struct SDLGUI_Element *panel, struct SDLGUI_Element *element);

#endif /*SDLGUI_PANEL_H*/
