#ifndef SDLGUI_ELEMENT_H
#define SDLGUI_ELEMENT_H

#include <SDL.h>
struct SDLGUI_Element;

#include "sdl2gui-list.h"

typedef void (SDLGUI_Clicked)(struct SDLGUI_Element *);
typedef void (SDLGUI_Destructor)(struct SDLGUI_Element *);
typedef void (SDLGUI_DrawFun)(struct SDLGUI_Element *elem);
typedef struct SDLGUI_Element* (SDLGUI_MouseHandler)(struct SDLGUI_Element *panel, int x, int y);
typedef void (SDLGUI_MouseDownFunction)(SDL_MouseButtonEvent *event);
typedef int (SDLGUI_MouseMotionFunction)(SDL_MouseMotionEvent *event);

struct SDLGUI_Element {
	SDL_Rect rect;
	struct SDLGUI_Element *parent;

	union {
		struct {
			struct SDLGUI_List *elements;
			int scrollY;
			int contentHeight;
		} panel;
		struct SDLGUI_Text_Data *textData;
		struct SDLGUI_Checkbox_Data *check;
		struct SDLGUI_Virtual_Data *virtualData;
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
	SDLGUI_MouseDownFunction *mouseDown;
	SDLGUI_MouseMotionFunction *mouseMotion;
	void (*textInputHandler)( struct SDLGUI_Element *e, char c, int backspace);
	void (*resizeHandler)(struct SDLGUI_Element *e, int width, int height);
	int (*scrollHandler)(struct SDLGUI_Element *e, int verticalScroll);

	int isVisible:1;
	int sizeHints:2;

	void *userData;
};

typedef struct {
	int r, g, b, a;
} SDLGUI_Color;

typedef struct {
	SDLGUI_Color bgColor;
	SDLGUI_Color fgColor;

	int fontWidth, fontHeight;

	int borderThickness;

	int isHidden:1;

	int horizontalAlignment:1;
} SDLGUI_Params;


#define INVISIBLE 0
#define VISIBLE 1
#define DEFAULT_VISIBLITY SDLGUI_VISIBLE

#define ALIGN_CENTER 0
#define ALIGN_LEFT 1

#define SDLGUI_SIZE_FILL -1

#define SDLGUI_SIZEHINTS_FIXED 0
#define SDLGUI_SIZEHINTS_STRETCH_HORIZONTAL 1
#define SDLGUI_SIZEHINTS_STRETCH_VERTICAL 2

#define SDLGUI_Destroy_Element( element) (element)->destructor( element)

SDL_Texture *createElementTexture( int width, int height, SDLGUI_Color bgColor, SDLGUI_Color borderColor, int borderThickness, SDL_Texture *fgTexture, int fgWidth, int fgHeight, int horizontalAlignment);
struct SDLGUI_Element *SDLGUI_Create_Texture( SDL_Rect rect, /*const*/SDL_Texture *texture, int textureWidth, int textureHeight, SDLGUI_Params params);
void SDLGUI_Texture_SetTextures( struct SDLGUI_Element *e, /*const*/SDL_Texture *texture, int textureWidth, int textureHeight, SDLGUI_Params params);

void SDLGUI_Draw_Texture( struct SDLGUI_Element *element);
void SDLGUI_Destroy_Texture( struct SDLGUI_Element *element);


struct SDLGUI_Element *SDLGUI_CreateElement(SDL_Rect rect);

#endif 
