#include "filePicker.h"

#include "../sdl2gui.h"
#include "dirUtils.h"

#include <string.h>
#include <stdio.h>

struct {
	void (*successCallback)(const char *path);
	void (*cancelCallback)();
	char *wantedFileName;
} filePickerData;

struct SDLGUI_Element *textbox_dir;
struct SDLGUI_Element *panel_dir;

struct MyDirectory *currentDir;

#define PANEL_WIDTH 290


void openDir(const char *path);

void minimizePath(char *path) {
	int i = 0;
	int slashCount = 0;

	for (i=0; path[i] != '\0'; i++) {
		if (path[i] == '/')
			slashCount ++;
	}

	char **tokens = (char **)calloc(slashCount, sizeof(char*));

	char *token = strtok(path, "/");
	i = 0;
	while (token) {
		if (i == 0 || strcmp(token, ".") != 0) {
			tokens[i] = token;
			i ++;
		}
		token = strtok(0, "/");
	}

	slashCount = i;

	if (slashCount > 2
		&& strcmp(tokens[slashCount - 1], "..") == 0
		&& strcmp(tokens[slashCount - 2], "..") != 0
	) {
		slashCount -= 2;
	}

	sprintf(path, "%s", tokens[0]);
	for (i=1; i<slashCount; i++) {
		sprintf(path, "%s/%s", path, tokens[i]);
	}
	sprintf(path, "%s/", path);

	free(tokens);
}

void destroyFilePicker() {
	if(filePickerData.wantedFileName) {
		free(filePickerData.wantedFileName);
	}

	SDLGUI_Layer_Remove();

	textbox_dir = 0;
}

void aFile_Clicked( struct SDLGUI_Element *e) {
	struct MyFile *f = (struct MyFile*)e->userData;

	if(f->isDir) {
		char buf[255];

		sprintf(buf, "%s%s/", SDLGUI_GetText_Textbox(textbox_dir), f->name);

		destroyDirectory(currentDir);

		openDir(buf);
	}
}

void openDir(const char *path) {
	currentDir = crossplatformDir(path, filePickerData.wantedFileName);

	if(currentDir == 0) {
		SDLGUI_Show_Message(SDLGUI_MESSAGE_ERROR, "Cannot open directory");
		return;
	}

	SDLGUI_Clear_Panel(panel_dir);

	SDLGUI_Params textParams = {
		.fontWidth = 6,
		.fontHeight = 8,
		.bgColor = COLOR_TRANSPARENT,
		.fgColor = COLOR_WHITE,
		.horizontalAlignment = ALIGN_LEFT
	};

	char buf[255];
	int i;
	const int lineHeight = textParams.fontHeight + 5;
	struct MyFile *file;
	for(i=0; i<currentDir->count; i++) {
		file = &currentDir->files[i];
		sprintf(buf, "%s%s", file->name, file->isDir ? "/" : "");

		struct SDLGUI_Element *e = SDLGUI_Create_Text(
			(SDL_Rect){.x=5, .y=5 + i * lineHeight, .w = PANEL_WIDTH -10, .h=lineHeight},
			buf,
			textParams
		);
		e->userData = file;
		e->clicked = aFile_Clicked;

		SDLGUI_AddTo_Panel( panel_dir, e);
	}

	sprintf(buf, "%s", path);

	minimizePath(buf);

	SDLGUI_SetText_Textbox( textbox_dir, buf);
}

void button_up_Clicked( struct SDLGUI_Element *e) {
	SDLGUI_Clear_Panel( panel_dir);
}

void button_cancel_Clicked( struct SDLGUI_Element *e) {
	destroyFilePicker();

	if (filePickerData.cancelCallback != 0) {
		filePickerData.cancelCallback();
	}
}

void button_open_Clicked( struct SDLGUI_Element *e) {
	const char* text = SDLGUI_GetText_Textbox(textbox_dir);

	char buf[256];

	sprintf(buf, "%s%s", text, filePickerData.wantedFileName);

	FILE *fp = fopen(buf, "r");

	if(fp == 0) {
		SDLGUI_Show_Message(SDLGUI_MESSAGE_ERROR, "Cannot open file");
		return;
	}

	fclose(fp);

	filePickerData.successCallback(buf);

	destroyFilePicker();
}



void initFilePicker() {
	SDLGUI_Color textColor = COLOR_WHITE;
	SDLGUI_Params buttonParams = {
		.bgColor = COLOR_BLACK,
		.fgColor = textColor,
		.borderThickness = 1,
		.fontWidth = 8,
		.fontHeight = 12
	};

	struct SDLGUI_List *elems = SDLGUI_List_Create(3);

	struct SDLGUI_Element *panel = SDLGUI_Create_Panel(
		(SDL_Rect){.x = 0, .y = 0, .w = SDLGUI_SIZE_FILL, .h=400},
		(SDLGUI_Params){
			.bgColor = (SDLGUI_Color){.r=0, .g=50, .b=100, .a=255},
			.fgColor = COLOR_WHITE,
			.borderThickness = 3
		}
	);


	struct SDLGUI_Element *label_title = SDLGUI_Create_Text(
		(SDL_Rect){.x = 10, .y = 10, .w = TEXT_SPAN_SIZE, .h = 30},
		"The Infamous  File Picker",
		(SDLGUI_Params){
			.bgColor = COLOR_TRANSPARENT,
			.fgColor = textColor,
			.fontWidth = 12,
			.fontHeight = 18
		}
	);


	struct SDLGUI_Element *button_up = SDLGUI_Create_Text(
		(SDL_Rect){.x = 10, .y = 50, .w = 80, .h = 30},
		"../",
		buttonParams
	);
	button_up->clicked = &button_up_Clicked;


	textbox_dir = SDLGUI_Create_Textbox(
		(SDL_Rect){.x = 100, .y=50, .w = 200, .h = 30},
		buttonParams
	);
	textbox_dir->data.textData->acceptedChars = TEXTBOX_INPUT_NONE;


	panel_dir = SDLGUI_Create_Panel(
		(SDL_Rect){.x = 10, .y=100, .w = PANEL_WIDTH, .h = 250},
		(SDLGUI_Params){
			.bgColor = COLOR_TRANSPARENT,
			.fgColor = textColor,
			.borderThickness = 1
		}
	);


	struct SDLGUI_Element *button_open = SDLGUI_Create_Text(
		(SDL_Rect){.x = 10, .y = 360, .w = 80, .h = 30},
		"OPEN",
		buttonParams
	);
	button_open->clicked = &button_open_Clicked;


	struct SDLGUI_Element *button_cancel = SDLGUI_Create_Text(
		(SDL_Rect){.x = 100, .y = 360, .w = 80, .h = 30},
		"CANCEL",
		buttonParams
	);
	button_cancel->clicked = &button_cancel_Clicked;


	SDLGUI_AddTo_Panel( panel, label_title);
	SDLGUI_AddTo_Panel( panel, button_up);
	SDLGUI_AddTo_Panel( panel, panel_dir);
	SDLGUI_AddTo_Panel( panel, textbox_dir);
	SDLGUI_AddTo_Panel( panel, button_open);
	SDLGUI_AddTo_Panel( panel, button_cancel);

	SDLGUI_List_Add( elems, panel);

	SDLGUI_Layer_Add(elems);
}

void SDLGUI_OpenFilePicker( const char *initialDir, const char *wantedFileName,
	void (*successCallback)(const char *path), void (*cancelCallback)()) {

	if (successCallback == 0)
		return;

	filePickerData.successCallback = successCallback;
	filePickerData.cancelCallback = cancelCallback;
	filePickerData.wantedFileName = wantedFileName == 0 ? 0 : strdup(wantedFileName);

	initFilePicker();

	openDir(initialDir);
}
