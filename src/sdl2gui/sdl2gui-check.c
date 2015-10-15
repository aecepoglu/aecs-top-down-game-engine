#include <stdlib.h>
#include "sdl2gui-check.h"
#include "sdl2gui-text.h"

void SDLGUI_Checkbox_Clicked( struct SDLGUI_Element *e) {
	struct SDLGUI_Checkbox_Data *data = e->data.check;

	data->value = ! data->value;
	
	struct {
		SDL_Texture *normal, *hover, *focused;
	} tmp = {
		.normal = e->textures.normal,
		.hover = e->textures.hover,
		.focused = e->textures.focused
	};

	e->textures.normal = data->otherTextures.normal;
	e->textures.hover = data->otherTextures.hover;
	e->textures.focused = data->otherTextures.focused;
	e->textures.current = data->otherTextures.normal;

	data->otherTextures.normal = tmp.normal;
	data->otherTextures.hover = tmp.hover;
	data->otherTextures.focused = tmp.focused;

	if( data->checkChanged)
		data->checkChanged( e, data->value);
}

void SDLGUI_Destroy_Checkbox( struct SDLGUI_Element *e) {
	struct SDLGUI_Checkbox_Data *data = e->data.check;

	free(data);

	SDLGUI_Destroy_Texture(e);
}

struct SDLGUI_Element* SDLGUI_Create_Checkbox( SDL_Rect rect, SDLGUI_Params params) {
	if( rect.w == TEXT_SPAN_SIZE) {
		rect.w = params.fontWidth * 2 + params.borderThickness;
	}
	struct SDLGUI_Element *uncheckedElement = SDLGUI_Create_Text( rect, " ", params);
	struct SDLGUI_Element *checkedElement = SDLGUI_Create_Text( rect, "X", params);

	struct SDLGUI_Checkbox_Data *data = (struct SDLGUI_Checkbox_Data*)malloc( sizeof( struct SDLGUI_Checkbox_Data));
	data->value = 0;
	data->checkChanged = 0;

	data->otherTextures.normal = checkedElement->textures.normal;
	data->otherTextures.hover = checkedElement->textures.hover;
	data->otherTextures.focused = checkedElement->textures.focused;

	uncheckedElement->clicked = SDLGUI_Checkbox_Clicked;
	uncheckedElement->data.check = data;
	uncheckedElement->destructor = SDLGUI_Destroy_Checkbox;

	free( checkedElement);

	return uncheckedElement;
}

int SDLGUI_Checkbox_GetValue( struct SDLGUI_Element *e) {
	struct SDLGUI_Checkbox_Data *data = e->data.check;

	return data->value;
}

void SDLGUI_Checkbox_SetValue( struct SDLGUI_Element *e, int value) {
	struct SDLGUI_Checkbox_Data *data = e->data.check;

	if( data->value != value) {
		SDLGUI_Checkbox_Clicked( e);
	}
}

void SDLGUI_Checkbox_SetOnCheckChanged( struct SDLGUI_Element *e, SDLGUI_Checkbox_Changed *checkChanged) {
	struct SDLGUI_Checkbox_Data *data = e->data.check;
	
	data->checkChanged = checkChanged;
}
