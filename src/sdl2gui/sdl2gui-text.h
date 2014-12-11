#include "sdl2gui-element.h"

#define DEFAULT_FONT_WIDTH 6
#define DEFAULT_FONT_HEIGHT 8

#define TEXT_SPAN_SIZE 0

struct SDLGUI_Element* SDLGUI_Create_Text( SDL_Rect rect, const char *text, SDLGUI_Params params);
void SDLGUI_Text_SetText( struct SDLGUI_Element *e, const char *text, SDLGUI_Params params);
