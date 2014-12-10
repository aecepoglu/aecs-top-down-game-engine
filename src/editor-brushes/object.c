#include "object.h"
#include "brush.h"
#include "../map.h"

void clicked_obj_player			( struct SDLGUI_Element *e) { brush.variant = go_player			; }
void clicked_obj_monster		( struct SDLGUI_Element *e) { brush.variant = go_leftTurner		; }
void clicked_obj_apple			( struct SDLGUI_Element *e) { brush.variant = go_apple			; }
void clicked_obj_door			( struct SDLGUI_Element *e) { brush.variant = go_door			; }
void clicked_obj_flower			( struct SDLGUI_Element *e) { brush.variant = go_flower			; }
void clicked_obj_creeperPlant	( struct SDLGUI_Element *e) { brush.variant = go_creeperPlant	; }
void clicked_obj_peekaboo		( struct SDLGUI_Element *e) { brush.variant = go_peekaboo		; }
void clicked_obj_weepingAngel	( struct SDLGUI_Element *e) { brush.variant = go_weepingAngel	; }
void clicked_obj_button			( struct SDLGUI_Element *e) { brush.variant = go_button			; }
void clicked_obj_sw				( struct SDLGUI_Element *e) { brush.variant = go_switch			; }
void clicked_obj_lineSensor		( struct SDLGUI_Element *e) { brush.variant = go_lineSensor		; }
void clicked_obj_box			( struct SDLGUI_Element *e) { brush.variant = go_box			; }
void clicked_obj_gate			( struct SDLGUI_Element *e) { brush.variant = go_gate			; }
void clicked_obj_key			( struct SDLGUI_Element *e) { brush.variant = go_key			; }

struct SDLGUI_Element* brushOptionPanel_create_object( struct SDLGUI_Element *parentPanel, SDLGUI_Params *panelParams, SDLGUI_Params *buttonParams, struct TextureSheet **objTextureSheets) {
	struct SDLGUI_Element *panel = SDLGUI_Create_Panel( (SDL_Rect){.x=0, .y=0, .w=parentPanel->rect.w, .h=160}, *panelParams);
	SDLGUI_AddTo_Panel( parentPanel, panel);

	SDLGUI_Clicked* onClickFunctions[] = {
		&clicked_obj_player,
		&clicked_obj_monster,
		&clicked_obj_apple,
		&clicked_obj_door,
		&clicked_obj_flower,
		&clicked_obj_creeperPlant,
		&clicked_obj_peekaboo,
		&clicked_obj_weepingAngel,
		&clicked_obj_button,
		&clicked_obj_sw,
		&clicked_obj_lineSensor,
		&clicked_obj_box,
		&clicked_obj_gate,
		&clicked_obj_key,
	};

	const int buttonsPerRow = 4;
	int i;
	for( i=0; i<go_NUM_ITEMS; i++) {
		struct SDLGUI_Element *button = SDLGUI_Create_Texture( (SDL_Rect){.x= 8 + (BUTTON_SIZE +8)* (i % buttonsPerRow), .y=8 + (BUTTON_SIZE +8)* (i / buttonsPerRow), .w=BUTTON_SIZE, .h=BUTTON_SIZE}, objTextureSheets[ i]->textures[1][0], ICON_SIZE, ICON_SIZE, *buttonParams);
		button->clicked = onClickFunctions[i];
		SDLGUI_AddTo_Panel( panel, button);
	}

	return panel;
}
