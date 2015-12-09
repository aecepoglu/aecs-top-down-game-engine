#ifndef SDL2GUI_H
#define SDL2GUI_H

#include <SDL.h>

#include "sdl2gui-element.h"
#include "sdl2gui-list.h"
#include "sdl2gui-text.h"
#include "sdl2gui-panel.h"
#include "sdl2gui-textbox.h"
#include "sdl2gui-check.h"
#include "sdl2gui-virtual.h"

/*
*/

#define COLOR_WHITE (SDLGUI_Color){.r=255, .g=255, .b=255, .a=255}
#define COLOR_BLACK (SDLGUI_Color){.r=0, .g=0, .b=0, .a=255}
#define COLOR_GRAY (SDLGUI_Color){.r=170, .g=180, .b=190, .a=255}
#define COLOR_TRANSPARENT (SDLGUI_Color){.r=0, .g=0, .b=0, .a=0}
#define DEFAULT_FGCOLOR COLOR_BLACK
#define DEFAULT_BGCOLOR COLOR_TRANSPARENT

enum SDLGUI_Message_Type {
	SDLGUI_MESSAGE_INFO,
	SDLGUI_MESSAGE_WARNING,
	SDLGUI_MESSAGE_ERROR,
};

struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture **font;

	struct SDLGUI_Element *mouseDownElement;
	struct SDLGUI_Element *mouseHoverElement;
	struct SDLGUI_Element *focusedElement;
	struct SDLGUI_Element *messageBox;
	struct SDLGUI_Element *tooltip;
	SDL_TimerID tooltipTimer;
} guiCore;


void SDLGUI_Init( SDL_Window *window, SDL_Renderer *renderer, SDL_Texture **font);
void SDLGUI_Draw();
void SDLGUI_Destroy();
int SDLGUI_Handle_MouseDown( SDL_MouseButtonEvent *e);
void SDLGUI_Handle_MouseUp( SDL_MouseButtonEvent *e);
int SDLGUI_Handle_MouseHover( SDL_MouseMotionEvent *e);
int SDLGUI_Handle_Scroll (const SDL_MouseWheelEvent *e);
int SDLGUI_Handle_TextInput( SDL_KeyboardEvent *e);


void SDLGUI_Destroy_Texture( struct SDLGUI_Element *element);

void SDLGUI_Show_Message( enum SDLGUI_Message_Type msgType, const char *text);
void SDLGUI_Hide_Message();

void SDLGUI_Show_Tooltip( int xPos, int yPos, const char *text);

void SDLGUI_Layer_Add( struct SDLGUI_List *list);
void SDLGUI_Layer_Remove();

void SDLGUI_Resize(int x, int y);
void SDLGUI_Refresh();

#endif /*SDL2GUI_H*/
