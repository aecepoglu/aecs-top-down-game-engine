#ifndef SDLGUI_ELEMENT_H
#define SDLGUI_ELEMENT_H

#include <SDL.h>
struct SDLGUI_Element;

#include "sdl2gui-list.h"

typedef void (SDLGUI_Clicked)(struct SDLGUI_Element *);
typedef void (SDLGUI_Destructor)(struct SDLGUI_Element *);
typedef void (SDLGUI_DrawFun)(struct SDLGUI_Element *elem);
typedef void (SDLGUI_TextChanged)(struct SDLGUI_Element *textbox, const char *text);
typedef struct SDLGUI_Element* (SDLGUI_MouseHandler)(struct SDLGUI_Element *panel, int x, int y);

struct SDLGUI_Element {
	SDL_Rect rect;

	union {
		struct SDLGUI_List *elements;
		struct SDLGUI_Text_Data *textData;
	} data;

	struct {
		SDL_Texture *current;
		SDL_Texture *normal;
		SDL_Texture *hover;
		SDL_Texture *focused;
	} textures;

	SDLGUI_Clicked *clicked;
	SDLGUI_Destructor *destructor;
	SDLGUI_DrawFun *drawFun;
	SDLGUI_MouseHandler *mouseHandler;

	int isVisible;
};

typedef struct {
	int r, g, b, a;
} SDLGUI_Color;

typedef struct {
	SDLGUI_Color bgColor;
	SDLGUI_Color fgColor;
	
	int isVisible;

	int fontWidth, fontHeight;

	int borderThickness;

	int isHidden;
} SDLGUI_Params;


#define INVISIBLE 0
#define VISIBLE 1
#define DEFAULT_VISIBLITY SDLGUI_VISIBLE


#define SDLGUI_Destroy_Element( element) (element)->destructor( element)

SDL_Texture *createElementTexture( int width, int height, SDLGUI_Color bgColor, SDLGUI_Color borderColor, int borderThickness, SDL_Texture *fgTexture, int fgWidth, int fgHeight);

void SDLGUI_Draw_Texture( struct SDLGUI_Element *element);
void SDLGUI_Destroy_Texture( struct SDLGUI_Element *element);

#endif 
