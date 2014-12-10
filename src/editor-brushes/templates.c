#include "templates.h"
#include "brush.h"
#include "../templates.h"

struct object **selectedObjectPtr;

struct TemplateButtonData {
	struct SDLGUI_Element *parent;
	int variant;
};

void repositionTemplateButtons( struct SDLGUI_List *list, int offsetY) {
	int i;
	for( i=2; i<list->count; i++) {
		list->list[i]->rect.y = offsetY + 52 + 20*(i-2);
	}
}

void template_clicked( struct SDLGUI_Element *e) {
	brush.variant = ((struct TemplateButtonData*)(e->userData))->variant;
}

void createTemplateButton( struct SDLGUI_Element *panel, int templateIndex, struct ObjectTemplate *t, SDLGUI_Params *buttonParams) {
	struct SDLGUI_Element *button = SDLGUI_Create_Text( (SDL_Rect){.x= 10, .w=panel->rect.w - 2*10, .h=20}, t->name, *buttonParams);
	button->clicked = template_clicked;

	struct TemplateButtonData *userData = (struct TemplateButtonData*)malloc( sizeof( struct TemplateButtonData));
	userData->variant  = templateIndex;
	userData->parent = panel;
	button->userData = userData;

	SDLGUI_AddTo_Panel( panel, button);
}



void addTemplate_clicked( struct SDLGUI_Element *e) {
	if( *selectedObjectPtr == NULL)
		return;
	struct SDLGUI_Element *panel = (struct SDLGUI_Element*)e->userData;
	int i;
	for( i=0; i<MAX_TEMPLATES_COUNT; i++) {
		if( objectTemplates[i] == NULL) {
			struct ObjectTemplate *template = template_create( i, "no name", 0, *selectedObjectPtr);
			
			objectTemplates[i] = template;
			SDLGUI_Params params = (SDLGUI_Params){
				.bgColor = COLOR_BLACK,
				.fgColor = COLOR_WHITE,
				.fontWidth = 6,
				.fontHeight = 8,
				.borderThickness = 1,
			};
			createTemplateButton( panel, i, template, &params);

			repositionTemplateButtons( panel->data.elements, panel->rect.y);
			break;
		}
	}
}

void removeTemplate_clicked( struct SDLGUI_Element *from) {
	struct SDLGUI_Element *panel = from->userData;
	struct SDLGUI_List *elements = panel->data.elements;
	int i;
	for( i=2; i<elements->count; i++) {
		struct SDLGUI_Element *e = elements->list[i];
		struct TemplateButtonData *templateData = (struct TemplateButtonData*)e->userData;
		if( templateData->variant == brush.variant) {
			SDLGUI_List_Remove( elements, e);
			repositionTemplateButtons( elements, panel->rect.y);
			free( e);
			template_remove( templateData->variant);
			free( templateData);
			break;
		}
	}; 
}

struct SDLGUI_Element* brushOptionPanel_create_templates( struct SDLGUI_Element *parentPanel, SDLGUI_Params *panelParams, SDLGUI_Params *buttonParams, struct object **_selectedPtr) {
	selectedObjectPtr = _selectedPtr;

	struct SDLGUI_Element *panel = SDLGUI_Create_Panel( (SDL_Rect){.x=0, .y=0, .w=parentPanel->rect.w, .h=230}, *panelParams);
	SDLGUI_AddTo_Panel( parentPanel, panel);

	struct SDLGUI_Element *button_add = SDLGUI_Create_Text( (SDL_Rect){.x=10, .y=10, .w=BUTTON_SIZE, .h=BUTTON_SIZE}, "+", *buttonParams);
	struct SDLGUI_Element *button_remove = SDLGUI_Create_Text( (SDL_Rect){.x=52, .y=10, .w=BUTTON_SIZE, .h=BUTTON_SIZE}, "-", *buttonParams);
	SDLGUI_AddTo_Panel( panel, button_add);
	SDLGUI_AddTo_Panel( panel, button_remove);
	button_add->clicked = addTemplate_clicked;
	button_add->userData = panel;
	button_remove->clicked = removeTemplate_clicked;
	button_remove->userData = panel;

	int i, drawI;
	for( i=0, drawI=0; i<MAX_TEMPLATES_COUNT; i++) {
		struct ObjectTemplate *t = objectTemplates[i];
		if( t == NULL)
			continue;

		drawI++;

		createTemplateButton( panel, i, t, buttonParams);
	}
	repositionTemplateButtons( panel->data.elements, panel->rect.y);

	return panel;
}
