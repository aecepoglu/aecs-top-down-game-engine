#include "brush.h"
#include "rotate.h"
#include "terrain.h"
#include "object.h"
#include "ai.h"

void brushOptionPanels_init( struct SDLGUI_Element *panel) {
	SDLGUI_Params panelParams = (SDLGUI_Params) {
		.bgColor = (SDLGUI_Color){255, 255, 0, 64},
		.fgColor = COLOR_BLACK,
		.borderThickness = 1,
		.isHidden = true
	};
	SDLGUI_Params buttonParams = (SDLGUI_Params) {
		.bgColor = COLOR_WHITE,
		.fgColor = COLOR_BLACK,
		.borderThickness = 1,
		.fontWidth = 12,
		.fontHeight= 16,
	};
	SDLGUI_Params textButtonParams = buttonParams;
	textButtonParams.fontWidth = 9;
	textButtonParams.fontHeight = 12;

	brushOptionPanels.rotate = brushOptionPanel_create_rotate( panel, &panelParams, &buttonParams);
	brushOptionPanels.terrain = brushOptionPanel_create_terrain( panel, &panelParams, &buttonParams);
	brushOptionPanels.object = brushOptionPanel_create_object( panel, &panelParams, &buttonParams);
	brushOptionPanels.ai = brushOptionPanel_create_ai( panel, &panelParams, &textButtonParams);
}