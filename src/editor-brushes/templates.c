#include <stdlib.h>
#include <string.h>
#include "templates.h"
#include "brush.h"
#include "../templates.h"

struct object **selectedObjectPtr;
struct {
	struct SDLGUI_Element *list, *detail;
} panels;
struct SDLGUI_Element *textbox_templateName;
SDLGUI_Params templateBrushList_params;

#define SHOW_LIST_PANEL() { panels.list->isVisible=true; panels.detail->isVisible=false; }
#define SHOW_DETAIL_PANEL() { panels.list->isVisible=false; panels.detail->isVisible=true; }

void repositionTemplateButtons( struct SDLGUI_List *list, int offsetY) {
	int i;
	for( i=2; i<list->count; i++) {
		list->list[i]->rect.y = offsetY + 52 + 20*(i-2);
	}
}

void template_clicked( struct SDLGUI_Element *e) {
	brush.fun = editor_applyTemplate;
	brush.variant = *((int*)(e->userData));
}

void createTemplateButton( int templateIndex, const struct ObjectTemplate *t, SDLGUI_Params *buttonParams) {
	struct SDLGUI_Element *button = SDLGUI_Create_Text( (SDL_Rect){.x= 10, .w=panels.list->rect.w - 2*10, .h=20}, t->name, *buttonParams);
	button->clicked = template_clicked;

	int *variantPtr = (int*)malloc(sizeof(int));
	*variantPtr = templateIndex;
	button->userData = variantPtr;

	SDLGUI_AddTo_Panel( panels.list, button);
}



void addTemplate_clicked( struct SDLGUI_Element *e) {
    brush.fun = editor_selectObj;
	if( *selectedObjectPtr == NULL)
		return;
	struct SDLGUI_Element *panel = panels.list;
	int i;
	for( i=0; i<MAX_TEMPLATES_COUNT; i++) {
		if( objectTemplates[i] == NULL) {
			enum AIType aiType = ai_none;
			if( (*selectedObjectPtr)->ai != NULL)
				aiType = (*selectedObjectPtr)->ai->type;

			struct ObjectTemplate *template = template_create( i, "", aiType, *selectedObjectPtr);
			
			objectTemplates[i] = template;
			SDLGUI_Params params = (SDLGUI_Params){
				.bgColor = COLOR_BLACK,
				.fgColor = COLOR_WHITE,
				.fontWidth = 6,
				.fontHeight = 8,
				.borderThickness = 1,
			};
			createTemplateButton( i, template, &params);

			repositionTemplateButtons( panel->data.elements, panel->rect.y);
            brush.variant = i;
			SHOW_DETAIL_PANEL();
			break;
		}
	}
}

struct SDLGUI_Element* getSelectedBrushElement() {
	struct SDLGUI_Element *panel = panels.list;
	struct SDLGUI_List *elements = panel->data.elements;
	int i;
	for( i=2; i<elements->count; i++) {
		struct SDLGUI_Element *e = elements->list[i];
		int *variantPtr = (int*)e->userData;
		if( *variantPtr == brush.variant) {
			return e;
		}
	}; 
	return NULL;
}

void removeTemplate_clicked( struct SDLGUI_Element *from) {
	struct SDLGUI_Element *panel = panels.list;
	struct SDLGUI_List *elements = panel->data.elements;
    
    struct SDLGUI_Element *e = getSelectedBrushElement();
    if( e ) {
		SDLGUI_List_Remove( elements, e);
		repositionTemplateButtons( elements, panel->rect.y);
        int *variantPtr = (int*)e->userData;
		free( e);
        template_remove( *variantPtr);
		free( variantPtr);
		SHOW_LIST_PANEL();
	}
}

void editTemplate_clicked( struct SDLGUI_Element *from) {
	struct ObjectTemplate *currentTemplate = objectTemplates[brush.variant];
	if( currentTemplate != NULL) {
		SDLGUI_SetText_Textbox( textbox_templateName, currentTemplate->name);
		SHOW_DETAIL_PANEL();
	}
}

void applyTemplate_clicked( struct SDLGUI_Element *e) {
	struct ObjectTemplate *currentTemplate = objectTemplates[brush.variant];
    const char *newName = SDLGUI_GetText_Textbox( textbox_templateName);
    if( strlen( newName) <= MAX_TEMPLATE_NAME_LENGTH) {
        currentTemplate->name = strdup( newName);
        SDLGUI_Text_SetText( getSelectedBrushElement(), currentTemplate->name, templateBrushList_params);
        SHOW_LIST_PANEL();
    }
}

struct SDLGUI_Element* brushOptionPanel_create_templates( struct SDLGUI_Element *parentPanel, SDLGUI_Params *panelParams, SDLGUI_Params *buttonParams, struct object **_selectedPtr) {
	selectedObjectPtr = _selectedPtr;
	templateBrushList_params = *buttonParams;

	const int panelsHeight = 230;

	struct SDLGUI_Element *containerPanel = SDLGUI_Create_Panel( (SDL_Rect){.x=0, .y=0, .w=parentPanel->rect.w, .h=panelsHeight}, *panelParams);
	SDLGUI_AddTo_Panel( parentPanel, containerPanel);

	panels.list = SDLGUI_Create_Panel( (SDL_Rect){.x=0, .y=0, .w=parentPanel->rect.w, .h=panelsHeight}, *panelParams);
	panels.list->isVisible = true;
	panels.detail = SDLGUI_Create_Panel( (SDL_Rect){.x=0, .y=0, .w=parentPanel->rect.w, .h=panelsHeight}, *panelParams);
	panels.detail->isVisible = false;
	SDLGUI_AddTo_Panel( containerPanel, panels.list);
	SDLGUI_AddTo_Panel( containerPanel, panels.detail);

	/* List Panel */
	struct SDLGUI_Element *button_add = SDLGUI_Create_Text( (SDL_Rect){.x=10, .y=10, .w=BUTTON_SIZE, .h=BUTTON_SIZE}, "+", *buttonParams);
	struct SDLGUI_Element *button_edit = SDLGUI_Create_Text( (SDL_Rect){.x=52, .y=10, .w=BUTTON_SIZE, .h=BUTTON_SIZE}, "E", *buttonParams);
	SDLGUI_AddTo_Panel( panels.list, button_add);
	SDLGUI_AddTo_Panel( panels.list, button_edit);

	button_add->clicked = addTemplate_clicked;
	button_edit->clicked = editTemplate_clicked;

	int i, drawI;
	for( i=0, drawI=0; i<MAX_TEMPLATES_COUNT; i++) {
		struct ObjectTemplate *t = objectTemplates[i];
		if( t == NULL)
			continue;

		drawI++;

		createTemplateButton( i, t, buttonParams);
	}
	repositionTemplateButtons( panels.list->data.elements, panels.list->rect.y);


	/* Detail View Panel */
	SDLGUI_Params textParams = {
		.bgColor = COLOR_TRANSPARENT,
		.fgColor = COLOR_BLACK,
		.fontWidth = DEFAULT_FONT_WIDTH,
		.fontHeight = DEFAULT_FONT_HEIGHT
	};

	struct SDLGUI_Element *label = SDLGUI_Create_Text( (SDL_Rect){.x=10, .y=10}, "Template Name:", textParams);
	textbox_templateName = SDLGUI_Create_Textbox( (SDL_Rect){.x=10, .y=30, .w=panels.detail->rect.w - 20}, textParams);
	struct SDLGUI_Element *button_apply = SDLGUI_Create_Text( (SDL_Rect){.x=10, .y=50, .w=BUTTON_SIZE, .h=BUTTON_SIZE}, "O", *buttonParams);
	struct SDLGUI_Element *button_remove = SDLGUI_Create_Text( (SDL_Rect){.x=52, .y=50, .w=BUTTON_SIZE, .h=BUTTON_SIZE}, "X", *buttonParams);
	SDLGUI_AddTo_Panel( panels.detail, label );
	SDLGUI_AddTo_Panel( panels.detail, textbox_templateName );
	SDLGUI_AddTo_Panel( panels.detail, button_apply );
	SDLGUI_AddTo_Panel( panels.detail, button_remove );
	
	button_apply->clicked = applyTemplate_clicked;
	button_remove->clicked = removeTemplate_clicked;
    textbox_templateName->data.textData->acceptedChars = TEXTBOX_INPUT_ALPHABET | TEXTBOX_INPUT_NUMERIC;

	return containerPanel;
}
