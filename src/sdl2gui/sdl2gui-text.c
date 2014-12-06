#include "sdl2gui.h"
#include "sdl2gui-element.h"
#include "sdl2gui-text.h"
#include "../text.h"

struct SDLGUI_Element* SDLGUI_Create_Text( SDL_Rect rect, const char *text, SDLGUI_Params params) {
	struct SDLGUI_Element *e = (struct SDLGUI_Element*) malloc( sizeof( struct SDLGUI_Element));

	int textWidth, textHeight;
	SDL_Texture *textTexture = getTextTexture( guiCore.renderer, guiCore.font, text, params.fontWidth, params.fontHeight, (int[4]){0,0,0,0}, params.fgColor.r, params.fgColor.g, params.fgColor.b, &textWidth, &textHeight);
	SDL_Texture *textTexture_inverseColors = getTextTexture( guiCore.renderer, guiCore.font, text, params.fontWidth, params.fontHeight, (int[4]){0,0,0,0}, 255-params.fgColor.r, 255-params.fgColor.g, 255-params.fgColor.b, &textWidth, &textHeight);

	int width = rect.w;
	int height = rect.h;
	if( width == TEXT_SPAN_SIZE)
		width = textWidth;
	if( height == TEXT_SPAN_SIZE)
		height = textHeight;
	
	SDLGUI_Color bgColor = params.bgColor;
	SDLGUI_Color fgColor = params.fgColor;
	
	
	e->textures.normal = e->textures.current = createElementTexture( width, height, bgColor, fgColor, params.borderThickness, textTexture, textWidth, textHeight);

	e->textures.hover = createElementTexture( width, height, (SDLGUI_Color){
			.r = (fgColor.r + bgColor.r) / 2,
			.g = (fgColor.g + bgColor.g) / 2,
			.b = (fgColor.b + bgColor.b) / 2,
			.a = bgColor.a + (255 - bgColor.a) / 2
		}, fgColor, params.borderThickness, textTexture, textWidth, textHeight);
	
	
	e->textures.focused = createElementTexture( width, height, (SDLGUI_Color){
			.r = (fgColor.r + 3*bgColor.r) / 4,
			.g = (fgColor.g + 3*bgColor.g) / 4,
			.b = (fgColor.b + 3*bgColor.b) / 4,
			.a = bgColor.a + (255 - bgColor.a) / 4
		}, fgColor, params.borderThickness +2, textTexture, textWidth, textHeight);

	SDL_DestroyTexture( textTexture);
	SDL_DestroyTexture( textTexture_inverseColors);

	
	e->rect = (SDL_Rect){
		.x = rect.x,
		.y = rect.y,
		.w = width,
		.h = height
	};
	e->clicked = NULL;
	e->destructor = SDLGUI_Destroy_Texture;
	e->drawFun = SDLGUI_Draw_Texture;
	e->mouseHandler = NULL;
	e->textInputHandler = NULL;
	e->isVisible = !params.isHidden;

	return e;
}
