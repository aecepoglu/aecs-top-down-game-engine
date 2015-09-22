#ifndef BRUSH_OBJECT_H
#define BRUSH_OBJECT_H

#include "../sdl2gui/sdl2gui.h"
#include "../core/texture.h"

struct SDLGUI_Element* brushOptionPanel_create_object( struct SDLGUI_Element *parentPanel, SDLGUI_Params *panelParams, SDLGUI_Params *buttonParams, struct TextureSheet **objTextureSheets);

#endif
