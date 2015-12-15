#ifndef BRUSH_H
#define BRUSH_H

#include "../sdl2gui/sdl2gui.h"
#include "../basic.h"
#include "../core/texture.h"
#include "../core/object.h"

typedef bool (brushFun)(unsigned int, unsigned int, int);

struct {
	brushFun *fun;
	int variant;
	bool isRepeat;
} brush;

brushFun defaultBrush,
	editor_move,
	editor_applyTemplate,
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
		*ai,
		*templates,
		*texture,
		*move;
} brushOptionPanels;

extern SDLGUI_Params editorMiniButtonParams;


#define BUTTON_SIZE 32
#define ICON_SIZE 16

void brushOptionPanels_init( struct SDLGUI_Element *parentPanel, struct TextureSheet **objTextureSheets, SDL_Texture **trnTextures, struct object **selectedObjPtr, SDL_Renderer *renderer);

#endif /* BRUSH_H */
