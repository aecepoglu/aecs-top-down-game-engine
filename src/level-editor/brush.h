#ifndef BRUSH_H
#define BRUSH_H

#include "../sdl2gui/sdl2gui.h"
#include "../basic.h"
#include "../texture.h"

typedef bool (brushFun)(unsigned int, unsigned int, int);

struct {
	brushFun *fun;
	int variant;
} brush;

brushFun defaultBrush,
	drawObject,
	drawTerrain,
	drawPlayer,
	drawAI,
	setDirection,
	eraseObject,
	eraseAI;

struct {
	struct SDLGUI_Element
		*terrain,
		*rotate,
		*object,
		*ai;
} brushOptionPanels;


#define BUTTON_SIZE 32
#define ICON_SIZE 16

void brushOptionPanels_init( struct SDLGUI_Element *parentPanel, struct TextureSheet **objTextureSheets, SDL_Texture **trnTextures);

#endif /* BRUSH_H */
