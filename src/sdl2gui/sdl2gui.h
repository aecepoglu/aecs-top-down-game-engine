#ifndef SDL2GUI_H
#define SDL2GUI_H

#include <SDL.h>

#include "sdl2gui-list.h"

/*
*/
#define TEXTBOX_INPUT_NONE 0
#define TEXTBOX_INPUT_NUMERIC 1
#define TEXTBOX_INPUT_ALPHABET 2

struct SDLGUI_Element;

typedef void (SDLGUI_Clicked)(struct SDLGUI_Element *);
typedef void (SDLGUI_Destructor)(struct SDLGUI_Element *);
typedef void (SDLGUI_DrawFun)(struct SDLGUI_Element *elem, int offsetX, int offsetY);
typedef struct SDLGUI_Element* (SDLGUI_MouseDownHandler)(struct SDLGUI_Element *elem, SDL_MouseButtonEvent *event);
typedef void (SDLGUI_TextChanged)(struct SDLGUI_Element *textbox, const char *text);

struct SDLGUI_Element {
	SDL_Rect rect;

    void *data;

	SDLGUI_Clicked *clicked;
	SDLGUI_Destructor *destructor;
	SDLGUI_DrawFun *drawFun;
	SDLGUI_MouseDownHandler *mouseDownHandler;
	void *userData;
};

struct SDLGUI_Panel_Data {
	struct SDLGUI_List *elements;
    SDL_Texture *texture;
};

enum SDLGUI_Message_Type {
	SDLGUI_MESSAGE_INFO,
	SDLGUI_MESSAGE_WARNING,
	SDLGUI_MESSAGE_ERROR,
};


void SDLGUI_Init( SDL_Renderer *renderer, SDL_Texture **font);
void SDLGUI_Add_Element( struct SDLGUI_Element *element);
void SDLGUI_Remove_Element( struct SDLGUI_Element *element);
void SDLGUI_Draw();
void SDLGUI_Destroy();
int SDLGUI_Handle_MouseDown( SDL_MouseButtonEvent *e);
void SDLGUI_Handle_MouseUp( SDL_MouseButtonEvent *e);
int SDLGUI_Handle_TextInput( SDL_KeyboardEvent *e);


#define SDLGUI_Destroy_Element( element) (element)->destructor( element)

//void SDLGUI_Draw_Texture( struct SDLGUI_Element *element);
void SDLGUI_Destroy_Texture( struct SDLGUI_Element *element);

struct SDLGUI_Element* SDLGUI_Create_Text( int xPos, int yPos, int width, int height, SDLGUI_Clicked *clicked, const char *text, int bgColor[4], int fgColor[4], int fontWidth, int fontHeight, int borderThickness, void *userData);


//void SDLGUI_Draw_Panel( struct SDLGUI_Element *element);
void SDLGUI_Destroy_Panel( struct SDLGUI_Element *element);
struct SDLGUI_Element* SDLGUI_Create_Panel( int xPos, int yPos, int width, int height, int bgColor[4], int borderColor[4], int borderThickness);
#define SDLGUI_Get_Panel_Elements( elem) ((struct SDLGUI_Panel_Data *)((elem)->data))->elements
void SDLGUI_Set_Panel_Elements( struct SDLGUI_Element *elem, struct SDLGUI_List *list, int  destroy);

void SDLGUI_Show_Message( int xPos, int yPos, int width, int height, enum SDLGUI_Message_Type msgType, const char *text);
void SDLGUI_Hide_Message();

void SDLGUI_Show_Tooltip( int xPos, int yPos, const char *text);

struct SDLGUI_Element* SDLGUI_Create_Textbox( int xPos, int yPos, int textCap, int allowedChars, int bgColor[4], int textColor[4], int fontWidth, int fontHeight, SDLGUI_TextChanged *textChanged);
void SDLGUI_SetText_Textbox( struct SDLGUI_Element *textbox, char *text);
void SDLGUI_ClearText_Textbox( struct SDLGUI_Element *textbox);
const char* SDLGUI_GetText_Textbox( struct SDLGUI_Element *textbox);
#endif /*SDL2GUI_H*/
