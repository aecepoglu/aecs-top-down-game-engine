#ifndef BRUSH_TEMPLATES_H
#define BRUSH_TEMPLATES_H

#include "../object.h"
#include "../sdl2gui/sdl2gui.h"

struct SDLGUI_Element* brushOptionPanel_create_templates( struct SDLGUI_Element *parentPanel, SDLGUI_Params *panelParams, SDLGUI_Params *buttonParams, struct object **selectedObjPtr);

#endif
