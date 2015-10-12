#include "mapCreator.h"
#include "sdl2gui/sdl2gui.h"
#include "guiHelpers.h"
#include "levelEditor.h"

struct SDLGUI_Element *panel,
	*textbox_width,
	*textbox_height,
	*textbox_name;

void editor_createMap_clicked( struct SDLGUI_Element *from) {
	int width = parseText( SDLGUI_GetText_Textbox( textbox_width  ) );
	int height = parseText( SDLGUI_GetText_Textbox( textbox_height) );
	const char *text = SDLGUI_GetText_Textbox( textbox_name);
	if( width <= 0 || height <= 0) {
		SDLGUI_Show_Message( SDLGUI_MESSAGE_ERROR, "Width & height must be >= 0");
	}
	else if (strlen(text) < 1) {
		SDLGUI_Show_Message( SDLGUI_MESSAGE_ERROR, "Need a longer map name");
	}
	else {
		myMap = createNewMap( width, height);

		char fileNameBuf[20];
		sprintf( fileNameBuf, "%s.yz.map", text);
		myMap->filePath = strdup( fileNameBuf);
		
		drawBackground();
		draw();

		SDLGUI_Layer_Remove();
	}
}

void createMapCreator() {
	SDLGUI_Params labelParams = (SDLGUI_Params){ .bgColor=COLOR_TRANSPARENT, .fgColor=COLOR_WHITE, .fontWidth=12, .fontHeight=16 };

	SDLGUI_Params buttonParams = labelParams;
	buttonParams.borderThickness = 2;

	panel = SDLGUI_Create_Panel( (SDL_Rect){.x=100, .y=100, .w=300, .h=500}, (SDLGUI_Params){.bgColor=COLOR_BLACK} );

	struct SDLGUI_Element *button_createMap = SDLGUI_Create_Text( (SDL_Rect){.x=0, .y=250, .w=200, .h=100}, "create", buttonParams);
	button_createMap->clicked = editor_createMap_clicked;

	struct SDLGUI_Element *button_quit = SDLGUI_Create_Text( (SDL_Rect){.x=0, .y=400, .w=200, .h=50}, "quit", buttonParams);
	button_quit->clicked = buttonQuit_clicked;

	textbox_width  = SDLGUI_Create_Textbox( (SDL_Rect){.x=100, .y=100, .w=100, .h=30}, labelParams);
	textbox_height = SDLGUI_Create_Textbox( (SDL_Rect){.x=100, .y=130, .w=100, .h=30}, labelParams);
	textbox_name   = SDLGUI_Create_Textbox( (SDL_Rect){.x=100, .y=170, .w=100, .h=30}, labelParams);
	textbox_width->data.textData->acceptedChars  = TEXTBOX_INPUT_NUMERIC;
	textbox_height->data.textData->acceptedChars = TEXTBOX_INPUT_NUMERIC;
	textbox_name->data.textData->acceptedChars   = TEXTBOX_INPUT_NUMERIC | TEXTBOX_INPUT_ALPHABET;

	SDLGUI_AddTo_Panel( panel, SDLGUI_Create_Text( (SDL_Rect){.x=0, .y= 10, .h=30}, "Creating New Map\n----------------", labelParams) );
	SDLGUI_AddTo_Panel( panel, SDLGUI_Create_Text( (SDL_Rect){.x=0, .y=100, .h=30}, " width", labelParams) );
	SDLGUI_AddTo_Panel( panel, SDLGUI_Create_Text( (SDL_Rect){.x=0, .y=130, .h=30}, "height", labelParams) );
	SDLGUI_AddTo_Panel( panel, SDLGUI_Create_Text( (SDL_Rect){.x=0, .y=170, .h=30}, "  name", labelParams) );

	SDLGUI_AddTo_Panel( panel, textbox_width );
	SDLGUI_AddTo_Panel( panel, textbox_height);
	SDLGUI_AddTo_Panel( panel, textbox_name);

	SDLGUI_AddTo_Panel( panel, button_createMap );
	SDLGUI_AddTo_Panel( panel, button_quit );


	struct SDLGUI_List *list = SDLGUI_List_Create(1);
	SDLGUI_List_Add( list, panel);
	
	SDLGUI_Layer_Add( list);
}
