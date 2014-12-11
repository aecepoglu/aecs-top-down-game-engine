#include <stdlib.h>
#include <string.h>
#include "sdl2gui-textbox.h"
#include "sdl2gui-element.h"
#include "../text.h"

#define TEXTBOX_MAX_LENGTH 64

void SDLGUI_Destroy_Textbox( struct SDLGUI_Element *textbox) {
	free( textbox->data.textData->text);
	free( textbox->data.textData);
	free( textbox);
}

void SDLGUI_Textbox_Clicked( struct SDLGUI_Element *elem) {
}

struct SDLGUI_Element* SDLGUI_Create_Textbox( SDL_Rect rect, SDLGUI_Params params) {
	struct SDLGUI_Text_Data *data = (struct SDLGUI_Text_Data*)malloc( sizeof( struct SDLGUI_Text_Data));
	struct SDLGUI_Element *e = (struct SDLGUI_Element*)malloc( sizeof( struct SDLGUI_Element));

	int height = rect.h == TEXT_SPAN_SIZE
		? params.fontHeight*2
		: rect.h;
	*data = (struct SDLGUI_Text_Data) {
		.fontWidth = params.fontWidth,
		.fontHeight = params.fontHeight,
		.text = (char*)calloc( TEXTBOX_MAX_LENGTH, sizeof(char)),
		.textChanged = 0,
		.textColor = params.fgColor
	};

	e->rect = rect;
	e->rect.h = height;
	e->data.textData = data;
	e->drawFun = SDLGUI_Draw_Texture;
	e->destructor = SDLGUI_Destroy_Textbox;
	e->clicked = SDLGUI_Textbox_Clicked;
	e->mouseHandler = 0;

	e->textures.normal = e->textures.hover = e->textures.focused = 0;

	e->isVisible = !params.isHidden;
	e->textInputHandler = SDLGUI_ChangeText_Textbox;
	
	SDLGUI_SetText_Textbox( e, "");

	return e;
}

int SDLGUI_SetText_Textbox( struct SDLGUI_Element *textbox, char *text) {
	if( text && strlen(text) >= TEXTBOX_MAX_LENGTH) 
		return 0; //TODO throw an error instead

	struct SDLGUI_Text_Data *data = textbox->data.textData;

	sprintf( data->text, "%s", text);

	if( textbox->textures.normal != 0) {
		SDL_DestroyTexture( textbox->textures.normal);
		SDL_DestroyTexture( textbox->textures.hover);
		SDL_DestroyTexture( textbox->textures.focused);
	}

	int textWidth, textHeight;

	SDLGUI_Color inverseColor = (SDLGUI_Color) {
		.r = 255 - data->textColor.r,
		.g = 255 - data->textColor.g,
		.b = 255 - data->textColor.b,
		.a = 255
	};

	SDL_Texture *texture_normal = getTextTexture( guiCore.renderer, guiCore.font, data->text, data->fontWidth, data->fontHeight, (int[4]){0,0,0,0}, data->textColor.r, data->textColor.g, data->textColor.b, &textWidth, &textHeight);
	SDL_Texture *texture_focused = getTextTexture( guiCore.renderer, guiCore.font, data->text, data->fontWidth, data->fontHeight, (int[4]){0,0,0,0}, inverseColor.r, inverseColor.g, inverseColor.b, &textWidth, &textHeight);

	textbox->textures.normal = createElementTexture( textbox->rect.w, textbox->rect.h, inverseColor, data->textColor, /*borderThickness*/1, texture_normal, textWidth, textHeight);
	textbox->textures.hover = createElementTexture( textbox->rect.w, textbox->rect.h, (SDLGUI_Color) {
			.r = 196 - data->textColor.r/2,
			.g = 196 - data->textColor.g/2,
			.b = 196 - data->textColor.b/2,
			.a = 255
		}, data->textColor, /*borderThickness*/1, texture_normal, textWidth, textHeight);
	textbox->textures.focused = createElementTexture( textbox->rect.w, textbox->rect.h, data->textColor, inverseColor, /*borderThickness*/1, texture_focused, textWidth, textHeight);
	
	SDL_DestroyTexture( texture_normal);
	SDL_DestroyTexture( texture_focused);
	
	if( data->textChanged)
		data->textChanged( textbox, data->text);
	
	if( guiCore.focusedElement == textbox)
		textbox->textures.current = textbox->textures.focused;
	else if (guiCore.mouseHoverElement == textbox)
		textbox->textures.current = textbox->textures.hover;
	else
		textbox->textures.current = textbox->textures.normal;

	return 1;
}
			
void SDLGUI_ChangeText_Textbox( struct SDLGUI_Element *textbox, char c, int backspace) {
	struct SDLGUI_Text_Data *data = textbox->data.textData;

	char newText[TEXTBOX_MAX_LENGTH];
	sprintf( newText, "%s", data->text);

	int curLen = strlen(data->text);
	int changed = 0;

	if( backspace && (data->acceptedChars != TEXTBOX_INPUT_NONE)) {
		if( curLen > 0) {
			newText[ curLen-1] = '\0';
			changed = 1;
		}
	}
	else if ( (data->acceptedChars & TEXTBOX_INPUT_NUMERIC && c >= '0' && c <= '9') || (data->acceptedChars & TEXTBOX_INPUT_ALPHABET && c >= 'a' && c <= 'z') ){
		if( curLen < TEXTBOX_MAX_LENGTH-1) {
			newText[ curLen] = c;
			newText[ curLen + 1] = '\0';
			changed = 1;
		}
	}

	if( changed) {
		SDLGUI_SetText_Textbox( textbox, newText);
	}
}

const char* SDLGUI_GetText_Textbox( struct SDLGUI_Element *textbox) {
	return textbox->data.textData->text;
}
