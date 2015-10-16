#ifndef BRUSH_TEXTURE_H
#define BRUSH_TEXTURE_H

#include "../sdl2gui/sdl2gui.h"

void reloadTextureButtons();

struct SDLGUI_Element* brushOptionPanel_create_texture( struct SDLGUI_Element *parentPanel, SDLGUI_Params *panelParams);

#endif
