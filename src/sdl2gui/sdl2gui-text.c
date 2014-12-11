#include "sdl2gui.h"
#include "sdl2gui-element.h"
#include "sdl2gui-text.h"
#include "../text.h"

struct SDLGUI_Element* SDLGUI_Create_Text( SDL_Rect rect, const char *text, SDLGUI_Params params) {
	int textWidth, textHeight;
	SDL_Texture *textTexture = getTextTexture( guiCore.renderer, guiCore.font, text, params.fontWidth, params.fontHeight, (int[4]){0,0,0,0}, params.fgColor.r, params.fgColor.g, params.fgColor.b, &textWidth, &textHeight);

	if( rect.w == TEXT_SPAN_SIZE)
		rect.w = textWidth;
	if( rect.h == TEXT_SPAN_SIZE)
		rect.h = textHeight;
	
	struct SDLGUI_Element *result = SDLGUI_Create_Texture( rect, textTexture, textWidth, textHeight, params);
	
	SDL_DestroyTexture( textTexture);

	return result;
}

void SDLGUI_Text_SetText( struct SDLGUI_Element *e, const char *text, SDLGUI_Params params) {
	int textWidth, textHeight;
	SDL_Texture *textTexture = getTextTexture( guiCore.renderer, guiCore.font, text, params.fontWidth, params.fontHeight, (int[4]){0,0,0,0}, params.fgColor.r, params.fgColor.g, params.fgColor.b, &textWidth, &textHeight);

	if( e->rect.w == TEXT_SPAN_SIZE)
		e->rect.w = textWidth;
	if( e->rect.h == TEXT_SPAN_SIZE)
		e->rect.h = textHeight;
	
	SDL_DestroyTexture( e->textures.normal);
	SDL_DestroyTexture( e->textures.hover);
	SDL_DestroyTexture( e->textures.focused);

	SDLGUI_Texture_SetTextures( e, textTexture, textWidth, textHeight, params);

	SDL_DestroyTexture( textTexture);
}
