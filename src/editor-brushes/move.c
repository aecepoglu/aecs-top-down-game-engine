#include "move.h"
#include "../sdl2gui/sdl2gui.h"

struct SDLGUI_Element* brushOptionPanel_create_move( struct SDLGUI_Element *parentPanel, SDLGUI_Params *panelParams) {
	struct SDLGUI_Element *panel = SDLGUI_Create_Panel( (SDL_Rect){.x=0, .y=0, .w=parentPanel->rect.w, .h=100}, *panelParams);
	SDLGUI_AddTo_Panel( parentPanel, panel);

	char *text = 
	"Mouse-down on an object -\n"\
	" and move it around";

	SDLGUI_AddTo_Panel( panel, SDLGUI_Create_Text( (SDL_Rect){.x= 10, .y=10, .w=panel->rect.w - 20, .h=panel->rect.h-20}, text, (SDLGUI_Params) {
		.bgColor = COLOR_TRANSPARENT,
		.fgColor = COLOR_BLACK,
		.fontWidth = DEFAULT_FONT_WIDTH,
		.fontHeight = DEFAULT_FONT_HEIGHT,
		.borderThickness = 0
	}) );

	return panel;
}
