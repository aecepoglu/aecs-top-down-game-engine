#include "texture.h"
#include "brush.h"
#include "../core/texture.h"
#include "../levelEditor.h"

#define BUTTONS_PER_ROW 3


struct SDLGUI_List *buttons;


void texture_clicked( struct SDLGUI_Element *e) {
	brush.variant = *((int*)e->userData);
}

void destroyTextureButton( struct SDLGUI_Element *e) {
	free((int*)e->userData);
	SDLGUI_Destroy_Element( e);
}

void reloadTextureButtons() {
	SDLGUI_List_Clear( buttons, 1);

	int i;
	for (i=0; i<textures->objsCount; i++) {
		if(textures->obj[i] != NULL) {
			struct SDLGUI_Element *button = SDLGUI_Create_Texture(
				(SDL_Rect){
					.x=10 + (i % BUTTONS_PER_ROW) * (BUTTON_SIZE + 10),
					.y=10*2 + BUTTON_SIZE + (i / BUTTONS_PER_ROW) * (BUTTON_SIZE + 10), 
					.w=BUTTON_SIZE,
					.h=BUTTON_SIZE
				},
				textures->obj[i]->textures[1][dir_up], ICON_SIZE, ICON_SIZE, editorMiniButtonParams
			);

			int *intPtr = (int*)malloc(sizeof(int));
			*intPtr = i;

			button->userData = (void*)intPtr;

			button->clicked = texture_clicked;
			button->destructor = destroyTextureButton;

			SDLGUI_List_Add( buttons, button);
		}
	}
}

void textureSchedulerCallback(void* untypedData) {
	reloadTextureButtons();
}

void refresh_clicked(struct SDLGUI_Element *e) {
	if (texturePaths == NULL) {
		SDLGUI_Show_Message( SDLGUI_MESSAGE_WARNING,
			"Textures-Specification file haven't been loaded.\n"
			"\n"
			"You will be prompted to load one now.\n"
			"\n"
			"( If you are loading the map for the first time,\n"
			" put the map file next to the texture-spec file,\n"
			" it'll load automatically )\n"
		);

		//TODO openTextureSchedulerDialog( &textureSchedulerCallback);
	}
}

struct SDLGUI_Element* brushOptionPanel_create_texture( struct SDLGUI_Element *parentPanel, SDLGUI_Params *panelParams ) {
	struct SDLGUI_Element *panel = SDLGUI_Create_Panel(
		(SDL_Rect){.x=0, .y=0, .w=parentPanel->rect.w, .h=200},
		*panelParams
	);


	SDL_Texture *reloadTexture = loadTexture( guiCore.renderer, "res/editor/rotate.png");

	struct SDLGUI_Element *button_reload = SDLGUI_Create_Texture(
		(SDL_Rect){.x=parentPanel->rect.w - 10 - BUTTON_SIZE, .y=10, .w=BUTTON_SIZE, .h=BUTTON_SIZE},
		reloadTexture, ICON_SIZE, ICON_SIZE, editorMiniButtonParams
	);

	SDL_DestroyTexture(reloadTexture);

	button_reload->clicked = refresh_clicked;


	struct SDLGUI_Element *panel_buttons = SDLGUI_Create_Panel(
		(SDL_Rect){.x=0, .y=30, .w=parentPanel->rect.w, .h=170},
		(SDLGUI_Params){
			.bgColor = COLOR_TRANSPARENT,
			.isVisible = VISIBLE,
			.isHidden = 0,
			.borderThickness = 0
		}
	);


	SDLGUI_AddTo_Panel( parentPanel, panel);

	SDLGUI_AddTo_Panel( panel, button_reload);
	SDLGUI_AddTo_Panel( panel, panel_buttons);


	buttons = SDLGUI_Get_Panel_Elements( panel_buttons);

	reloadTextureButtons();


	return panel;
}
