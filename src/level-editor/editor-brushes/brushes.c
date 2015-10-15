#include "brush.h"
#include "rotate.h"
#include "terrain.h"
#include "ai.h"
#include "templates.h"
#include "move.h"
#include "texture.h"

SDLGUI_Params editorMiniButtonParams = {
	.bgColor = COLOR_WHITE,
	.fgColor = COLOR_BLACK,
	.borderThickness = 1,
	.fontWidth = 12,
	.fontHeight= 16
};

void brushOptionPanels_init( struct SDLGUI_Element *panel, struct TextureSheet **objTextureSheets, SDL_Texture **trnTextures, struct object **selectedObjPtr, SDL_Renderer *renderer) {
	SDLGUI_Params panelParams = (SDLGUI_Params) {
		.bgColor = (SDLGUI_Color){255, 255, 0, 64},
		.fgColor = COLOR_BLACK,
		.borderThickness = 1,
		.isHidden = true
	};
	SDLGUI_Params textButtonParams = editorMiniButtonParams;
	textButtonParams.fontWidth = 9;
	textButtonParams.fontHeight = 12;

	brushOptionPanels.rotate = brushOptionPanel_create_rotate( panel, &panelParams, &editorMiniButtonParams);
	brushOptionPanels.terrain = brushOptionPanel_create_terrain( panel, &panelParams, &editorMiniButtonParams, trnTextures);
	brushOptionPanels.ai = brushOptionPanel_create_ai( panel, &panelParams, &textButtonParams);
	brushOptionPanels.templates = brushOptionPanel_create_templates( panel, &panelParams, &editorMiniButtonParams, selectedObjPtr, renderer);
	brushOptionPanels.move = brushOptionPanel_create_move( panel, &panelParams);
	brushOptionPanels.texture = brushOptionPanel_create_texture( panel, &panelParams);
}
