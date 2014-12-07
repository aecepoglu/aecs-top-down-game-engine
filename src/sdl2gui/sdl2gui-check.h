#ifndef SDLGUI_CHECKBOX_H
#define SDLGUI_CHECKBOX_H

struct SDLGUI_Checkbox_Data;

#include "sdl2gui-element.h"

typedef void (SDLGUI_Checkbox_Changed)( struct SDLGUI_Element *e, int newVaLue);

struct SDLGUI_Checkbox_Data {
	SDLGUI_Checkbox_Changed *checkChanged;
	int value;

	struct {
		SDL_Texture *normal, *hover, *focused;
	} otherTextures;
};


struct SDLGUI_Element* SDLGUI_Create_Checkbox( SDL_Rect rect, SDLGUI_Params params);
int SDLGUI_Checkbox_GetValue( struct SDLGUI_Element *checkbox);
void SDLGUI_Checkbox_SetValue( struct SDLGUI_Element *checkbox, int value);
void SDLGUI_Checkbox_SetOnCheckChanged( struct SDLGUI_Element *checkbox, SDLGUI_Checkbox_Changed *checkChanged);

#endif /* SDLGUI_CHECKBOX_H */
