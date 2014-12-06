#include "terrain.h"
#include "brush.h"
#include "../map.h"

#define BUTTON_SIZE 32

void wall( struct SDLGUI_Element *e) {
	brush.variant = terrain_wall;
}
void ground( struct SDLGUI_Element *e) {
	brush.variant = terrain_gnd;
}
void marked( struct SDLGUI_Element *e) {
	brush.variant = terrain_marked;
}

struct SDLGUI_Element* brushOptionPanel_create_terrain( struct SDLGUI_Element *parentPanel, SDLGUI_Params *panelParams, SDLGUI_Params *buttonParams) {
	struct SDLGUI_Element *panel = SDLGUI_Create_Panel( (SDL_Rect){.x=0, .y=0, .w=parentPanel->rect.w, .h=52}, *panelParams);
	SDLGUI_AddTo_Panel( parentPanel, panel);

	struct SDLGUI_Element *button_gnd 	= SDLGUI_Create_Text( (SDL_Rect){.x= 10, .y=10, .w=BUTTON_SIZE, .h=BUTTON_SIZE}, "G", *buttonParams);
	struct SDLGUI_Element *button_wall	= SDLGUI_Create_Text( (SDL_Rect){.x= 52, .y=10, .w=BUTTON_SIZE, .h=BUTTON_SIZE}, "W", *buttonParams);
	struct SDLGUI_Element *button_marked= SDLGUI_Create_Text( (SDL_Rect){.x= 94, .y=10, .w=BUTTON_SIZE, .h=BUTTON_SIZE}, "M", *buttonParams);
	button_gnd->clicked = ground;
	button_wall->clicked = wall;
	button_marked->clicked = marked;

	SDLGUI_AddTo_Panel( panel, button_gnd);
	SDLGUI_AddTo_Panel( panel, button_wall);
	SDLGUI_AddTo_Panel( panel, button_marked);

	return panel;
}
