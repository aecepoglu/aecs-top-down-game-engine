#include "texture.h"
#include "brush.h"
#include "../core/texture.h"
#include "../levelEditor.h"
#include "../../sdl2gui/widgets/filePicker.h"
#include <stdio.h>

#define BUTTONS_PER_ROW 4
#define BUTTON_MARGIN 9


struct SDLGUI_Element *panel_buttons;


void texture_clicked( struct SDLGUI_Element *e) {
	brush.variant = *((int*)e->userData);
}

void destroyTextureButton( struct SDLGUI_Element *e) {
	free((int*)e->userData);
	SDLGUI_Destroy_Texture( e);
}

void reloadTextureButtons() {
	if (spriteSpecs) {
		loadObjectTextures( guiCore.renderer, textures, spriteSpecs);
	}

	SDLGUI_Clear_Panel( panel_buttons);

	int i;
	int count = 0;
	for (i=0; i<textures->objsCount; i++) {
		if(textures->obj[i] != NULL) {
			printf("obj %d exists\n", i);
			struct SDLGUI_Element *button = SDLGUI_Create_Texture(
				(SDL_Rect){
					.x = BUTTON_MARGIN + (count % BUTTONS_PER_ROW) * (BUTTON_SIZE + BUTTON_MARGIN),
					.y = (count / BUTTONS_PER_ROW) * (BUTTON_SIZE + BUTTON_MARGIN), 
					.w = BUTTON_SIZE,
					.h = BUTTON_SIZE
				},
				textures->obj[i]->textures[1][dir_up], ICON_SIZE, ICON_SIZE, editorMiniButtonParams
			);

			int *intPtr = (int*)malloc(sizeof(int));
			*intPtr = i;

			button->userData = (void*)intPtr;

			button->clicked = texture_clicked;
			button->destructor = destroyTextureButton;

			SDLGUI_AddTo_Panel( panel_buttons, button);

			count++;
		}
	}
}

void filePicked(const char *path) {
	spriteSpecs = readSpriteSpecsFile(path);

	reloadTextureButtons();
}

void refresh_clicked(struct SDLGUI_Element *e) {
	if (spriteSpecs == NULL) {
		SDLGUI_Show_Message( SDLGUI_MESSAGE_WARNING,
			"Textures-Specification file haven't been loaded.\n"
			"\n"
			"You will be prompted to load one now.\n"
			"\n"
			"( If you are loading the map for the first time,\n"
			" put the map file next to the texture-spec file,\n"
			" it'll load automatically )\n"
		);

		SDLGUI_OpenFilePicker("./", SPRITE_SPECS_FILENAME, &filePicked, NULL);
	}
	else {
		clearSpriteSpecs(spriteSpecs);
		FILE *fp = fopen(spriteSpecs->filePath, "r");

		if(fp == NULL) {
			log1("Reloading texture-schedule-specs file: %s\n", spriteSpecs->filePath);
			SDLGUI_Show_Message( SDLGUI_MESSAGE_WARNING,
				"File could not be loaded.\n"
				"You may have moved it elsewhere or removed it\n"
				" ... I mean it's your computer,\n"
				"\n"
				" How should I know?"
			);
			return;
		}

		loadSpriteSpecs(spriteSpecs, fp);

		fclose(fp);


		reloadTextureButtons();
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


	panel_buttons = SDLGUI_Create_Panel(
		(SDL_Rect){.x=0, .y=BUTTON_SIZE + 20, .w=parentPanel->rect.w, .h=200 - (BUTTON_SIZE + 20)},
		(SDLGUI_Params){
			.bgColor = COLOR_TRANSPARENT,
		}
	);


	SDLGUI_AddTo_Panel( parentPanel, panel);

	SDLGUI_AddTo_Panel( panel, button_reload);
	SDLGUI_AddTo_Panel( panel, panel_buttons);


	reloadTextureButtons();


	return panel;
}
