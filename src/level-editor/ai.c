#include "ai.h"
#include "brush.h"
#include "../ai.h"

#define BUTTON_SIZE 32

void clicked_ai_none				( struct SDLGUI_Element *e) { brush.variant = ai_none				; }
void clicked_ai_leftTurner			( struct SDLGUI_Element *e) { brush.variant = ai_leftTurner			; }
void clicked_ai_hungryLeftTurner	( struct SDLGUI_Element *e) { brush.variant = ai_hungryLeftTurner	; }
void clicked_ai_flower				( struct SDLGUI_Element *e) { brush.variant = ai_simpleFlower		; }
void clicked_ai_creeperPlant		( struct SDLGUI_Element *e) { brush.variant = ai_creeperPlant		; }
void clicked_ai_peekaboo			( struct SDLGUI_Element *e) { brush.variant = ai_peekaboo			; }
void clicked_ai_weepingAngel		( struct SDLGUI_Element *e) { brush.variant = ai_weepingAngel		; }
void clicked_ai_door				( struct SDLGUI_Element *e) { brush.variant = ai_door				; }
void clicked_ai_lineSensor			( struct SDLGUI_Element *e) { brush.variant = ai_lineSensor			; }
void clicked_ai_sw					( struct SDLGUI_Element *e) { brush.variant = ai_switch				; }

struct SDLGUI_Element* brushOptionPanel_create_ai( struct SDLGUI_Element *parentPanel, SDLGUI_Params *panelParams, SDLGUI_Params *buttonParams) {
	struct SDLGUI_Element *panel = SDLGUI_Create_Panel( (SDL_Rect){.x=0, .y=0, .w=parentPanel->rect.w, .h=230}, *panelParams);
	SDLGUI_AddTo_Panel( parentPanel, panel);


	SDLGUI_Clicked *templates[ai_NUM_ITEMS] = {
		clicked_ai_none,
		clicked_ai_leftTurner,
		clicked_ai_hungryLeftTurner,
		clicked_ai_flower,
		clicked_ai_creeperPlant,
		clicked_ai_peekaboo,
		clicked_ai_weepingAngel,
		clicked_ai_door,
		clicked_ai_lineSensor,
		clicked_ai_sw,
	};

	int i;
	for( i=0; i<ai_NUM_ITEMS; i++) {
		struct SDLGUI_Element *button = SDLGUI_Create_Text( (SDL_Rect){.x= 10, .y=10 + 20*i, .w=parentPanel->rect.w - 2*10, .h=20}, aiNames[i], *buttonParams);
		button->clicked = templates[i];
		SDLGUI_AddTo_Panel( panel, button);
	}

	return panel;
}
