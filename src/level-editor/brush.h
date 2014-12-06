#ifndef BRUSH_H
#define BRUSH_H

#include "../sdl2gui/sdl2gui.h"
#include "../basic.h"

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

void brushOptionPanels_init( struct SDLGUI_Element *panel);

#endif /* BRUSH_H */
