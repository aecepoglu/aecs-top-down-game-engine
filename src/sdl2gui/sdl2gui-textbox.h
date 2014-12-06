#ifndef SDLGUI_TEXTBOX_H
#define SDLGUI_TEXTBOX_H

struct SDLGUI_TextBox_Data;

#include "sdl2gui.h"
#include "sdl2gui-element.h"


typedef void (SDLGUI_TextBox_Changed)(struct SDLGUI_Element *textbox, const char *text);
typedef char* (SDLGUI_TextBox_Validator)(const char *text);

struct SDLGUI_Text_Data {
	char *text;
	
    int fontWidth, fontHeight;
	SDLGUI_Color textColor;
	SDLGUI_TextBox_Changed *textChanged;
	
    int acceptedChars;
};

#define TEXTBOX_INPUT_NONE 0
#define TEXTBOX_INPUT_NUMERIC 1
#define TEXTBOX_INPUT_ALPHABET 2


struct SDLGUI_Element* SDLGUI_Create_Textbox( SDL_Rect rect, SDLGUI_Params params);

int SDLGUI_SetText_Textbox( struct SDLGUI_Element *textbox, char *text);

//void SDLGUI_ClearText_Textbox( struct SDLGUI_Element *textbox);

const char* SDLGUI_GetText_Textbox( struct SDLGUI_Element *textbox);

void SDLGUI_ChangeText_Textbox( struct SDLGUI_Element *textbox, char c, int backspace);

#endif /* SDLGUI_TEXT_BOX_H */
