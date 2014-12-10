#include "rotate.h"
#include "../sdl2gui/sdl2gui.h"

#define BUTTON_SIZE 32

void up( struct SDLGUI_Element *e) {
	brush.variant = 0;
}
void right( struct SDLGUI_Element *e) {
	brush.variant = 1;
}
void down( struct SDLGUI_Element *e) {
	brush.variant = 2;
}
void left( struct SDLGUI_Element *e) {
	brush.variant = 3;
}

struct SDLGUI_Element* brushOptionPanel_create_rotate( struct SDLGUI_Element *parentPanel, SDLGUI_Params *panelParams, SDLGUI_Params *buttonParams) {
	struct SDLGUI_Element *panel = SDLGUI_Create_Panel( (SDL_Rect){.x=0, .y=0, .w=parentPanel->rect.w, .h=52}, *panelParams);
	SDLGUI_AddTo_Panel( parentPanel, panel);

	struct SDLGUI_Element *button_up	= SDLGUI_Create_Text( (SDL_Rect){.x= 10, .y=10, .w=BUTTON_SIZE, .h=BUTTON_SIZE}, "\x80", *buttonParams);
	struct SDLGUI_Element *button_right = SDLGUI_Create_Text( (SDL_Rect){.x= 52, .y=10, .w=BUTTON_SIZE, .h=BUTTON_SIZE}, "\x81", *buttonParams);
	struct SDLGUI_Element *button_down	= SDLGUI_Create_Text( (SDL_Rect){.x= 94, .y=10, .w=BUTTON_SIZE, .h=BUTTON_SIZE}, "\x82", *buttonParams);
	struct SDLGUI_Element *button_left	= SDLGUI_Create_Text( (SDL_Rect){.x=136, .y=10, .w=BUTTON_SIZE, .h=BUTTON_SIZE}, "\x83", *buttonParams);
	button_up->clicked = up;
	button_right->clicked = right;
	button_down->clicked = down;
	button_left->clicked = left;

	SDLGUI_AddTo_Panel( panel, button_up);
	SDLGUI_AddTo_Panel( panel, button_right);
	SDLGUI_AddTo_Panel( panel, button_down);
	SDLGUI_AddTo_Panel( panel, button_left);

	return panel;
}
