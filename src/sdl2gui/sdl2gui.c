#include <stdlib.h>
#include <string.h>

#include "sdl2gui.h"
#include "../text.h"
#include "../log.h"


struct SDLGUI_Core {
	SDL_Renderer *renderer;
	SDL_Texture **font;

	struct SDLGUI_List elements;

	struct SDLGUI_Element *mouseDownElement;
	struct SDLGUI_Element *focusedElement;
	struct SDLGUI_Element *messageBox;
	struct SDLGUI_Element *tooltip;
	SDL_TimerID tooltipTimer;
} guiCore;

/* Fwd declarations
*/
void SDLGUI_ChangeText_Textbox( struct SDLGUI_Element *textbox, char c, int backspace);


/* ---------------------
	SDLGUI Core
*/

void SDLGUI_Init( SDL_Renderer *renderer, SDL_Texture **font) {
	guiCore.renderer = renderer;
	guiCore.font = font;
	guiCore.mouseDownElement = 0;
	guiCore.messageBox = 0;
	guiCore.tooltip = 0;
	guiCore.focusedElement = 0;

	SDLGUI_List_Init( &guiCore.elements, 4);
}

void SDLGUI_Destroy() {
	SDLGUI_List_Destroy( &guiCore.elements, 0);
	guiCore.renderer = 0;
	guiCore.font = 0;

	if( guiCore.messageBox) {
		SDLGUI_Destroy_Element( guiCore.messageBox);
	}
}

void SDLGUI_Draw() {
	int i;
	for( i=0; i<guiCore.elements.count; i++) {
		guiCore.elements.list[i]->drawFun( guiCore.elements.list[i], 0, 0);
	}

	if( guiCore.messageBox)
		guiCore.messageBox->drawFun( guiCore.messageBox, 0, 0);

	if( guiCore.tooltip)
		guiCore.tooltip->drawFun( guiCore.tooltip, 0, 0);
}

struct SDLGUI_Element* SDLGUI_Handle_MouseDown_List( struct SDLGUI_List *list, SDL_MouseButtonEvent *e) {
	struct SDLGUI_Element *elem;
	int i;
	for(i=0; i<list->count; i++) {
		elem = list->list[i];
		if (e->x >= elem->rect.x && e->y >= elem->rect.y && e->x<elem->rect.x+elem->rect.w && e->y<elem->rect.y+elem->rect.h) {
			if( elem->mouseDownHandler != 0)
				return elem->mouseDownHandler( elem, e);
			else
				return elem;
		}
	}
	return 0;
}

int SDLGUI_Handle_MouseDown( SDL_MouseButtonEvent *e) {
	struct SDLGUI_Element *mouseDownElement = SDLGUI_Handle_MouseDown_List( &guiCore.elements, e);
	if( mouseDownElement != 0) {
		guiCore.mouseDownElement = mouseDownElement;
		return 1;
	}
	else
		return 0;
}

void SDLGUI_Handle_MouseUp( SDL_MouseButtonEvent *e) {
	if( guiCore.mouseDownElement && guiCore.mouseDownElement->clicked != 0) {
		guiCore.mouseDownElement->clicked( guiCore.mouseDownElement);
		guiCore.mouseDownElement = 0;
	}
	else
		guiCore.focusedElement = 0;
}

int SDLGUI_Handle_TextInput( SDL_KeyboardEvent *e) {
	if( guiCore.focusedElement) {
		SDL_Keycode sym = e->keysym.sym;
		char c = 0;
		int backspace = 0;
		log0("%d\n", e->keysym.sym);
		
		if( sym >= SDLK_0 && sym <= SDLK_z) {
			c = sym;
		}
		else if( sym == SDLK_BACKSPACE) {
			backspace = 1;
		}
		
		if( c != 0 || backspace != 0) {
			log0("input: %c, backspace: %d\n", c, backspace);
			SDLGUI_ChangeText_Textbox( guiCore.focusedElement, c, backspace);
			return 1;
		}
	}
	return 0;
};

void SDLGUI_Add_Element( struct SDLGUI_Element *element) {
	SDLGUI_List_Add( &guiCore.elements, element);
}

void SDLGUI_Remove_Element( struct SDLGUI_Element *element) {
	SDLGUI_List_Remove( &guiCore.elements, element);
}

/* ------------------
	SDLGUI Elements
*/

SDL_Texture *createElementTexture( int width, int height, int bgColor[4], int borderColor[4], int borderThickness, SDL_Texture *fgTexture, int fgWidth, int fgHeight) {
	SDL_Texture *texture = SDL_CreateTexture( guiCore.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height) ;
	SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND);


	SDL_SetRenderTarget( guiCore.renderer, texture);

	SDL_SetRenderDrawColor( guiCore.renderer, borderColor[0], borderColor[1], borderColor[2], borderColor[3]);
	SDL_RenderClear( guiCore.renderer);

	SDL_SetRenderDrawColor( guiCore.renderer, bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	SDL_Rect rect = {.x=borderThickness, .y=borderThickness, .w=width-2*borderThickness, .h=height-2*borderThickness};
	SDL_RenderFillRect( guiCore.renderer, &rect);

    if( fgTexture) {
	    //TODO assert width > fgWidth, height>fgHeight
	    rect.x = (width - fgWidth) / 2;
	    rect.y = (height - fgHeight) / 2;
	    rect.w = fgWidth;
	    rect.h = fgHeight;
	    SDL_RenderCopy( guiCore.renderer, fgTexture, NULL, &rect);
    }

	SDL_SetRenderTarget( guiCore.renderer, 0);

	return texture;
}

struct SDLGUI_Element* SDLGUI_Create_Element( int xPos, int yPos, int width, int height, void *data, SDLGUI_Clicked *clicked, SDLGUI_Destructor *dtor, SDLGUI_DrawFun *drawFun, SDLGUI_MouseDownHandler *mouseDownHandler, void *userData) {
	struct SDLGUI_Element *e = (struct SDLGUI_Element*) malloc( sizeof( struct SDLGUI_Element));

	e->rect.x = xPos;
	e->rect.y = yPos;
	e->rect.w = width;
	e->rect.h = height;
	e->data = data;

	e->clicked = clicked;
	e->destructor = dtor;
	e->drawFun = drawFun;
	e->mouseDownHandler = mouseDownHandler;
	e->userData = userData;

	e->isVisible = 1;

	return e;
}

/* Texture
*/

void SDLGUI_Draw_Texture( struct SDLGUI_Element *element, int offsetX, int offsetY){
	if( element->isVisible) {
		SDL_Rect rect = {element->rect.x, element->rect.y, element->rect.w, element->rect.h};
		rect.x += offsetX;
		rect.y += offsetY;
		SDL_RenderCopy( guiCore.renderer, (SDL_Texture*)element->data, NULL, &rect);
	}
}

struct SDLGUI_Element* SDLGUI_Create_Texture( int xPos, int yPos, int width, int height, SDL_Texture *texture, SDLGUI_Clicked *clicked, void *userData) {
	return SDLGUI_Create_Element( xPos, yPos, width, height, texture, clicked, &SDLGUI_Destroy_Texture, &SDLGUI_Draw_Texture, 0, userData);
}

void SDLGUI_Destroy_Texture( struct SDLGUI_Element *element) {
	SDL_Texture *t = (SDL_Texture*)element->data;
	SDL_DestroyTexture( t);
	free( element);
}

/* Text
*/

struct SDLGUI_Element* SDLGUI_Create_Text( int xPos, int yPos, int width, int height, SDLGUI_Clicked *clicked, const char *text, int bgColor[4], int fgColor[4], int fontWidth, int fontHeight, int borderThickness, void *userData) {
	
	int textWidth, textHeight;
	SDL_Texture *textTexture = getTextTexture( guiCore.renderer, guiCore.font, text, fontWidth, fontHeight, (int[4]){0,0,0,0}, fgColor[0], fgColor[1], fgColor[2], &textWidth, &textHeight);

	if( width < 0)
		width = textWidth;
	if( height < 0)
		height = textHeight;

	SDL_Texture *elementTexture = createElementTexture( width, height, bgColor, fgColor, borderThickness, textTexture, textWidth, textHeight);
	SDL_DestroyTexture( textTexture);

	return SDLGUI_Create_Texture( xPos, yPos, width, height, elementTexture, clicked, userData);
}

/* Panel
*/

void SDLGUI_Draw_Panel( struct SDLGUI_Element *element, int offsetX, int offsetY) {
	if( ! element->isVisible) 
		return;

    struct SDLGUI_Panel_Data *data = (struct SDLGUI_Panel_Data*)element->data;

	SDL_Rect rect = {element->rect.x + offsetX, element->rect.y + offsetY, element->rect.w, element->rect.h};
	
    SDL_RenderCopy( guiCore.renderer, data->texture, 0, &rect);

    int i;
    for( i=0; i<data->elements->count; i++) {
        struct SDLGUI_Element *e = data->elements->list[i];
        if( e->drawFun != 0)
            e->drawFun( e, offsetX + element->rect.x, offsetY + element->rect.y);
    }
}

void SDLGUI_Destroy_Panel( struct SDLGUI_Element *element) {
    struct SDLGUI_Panel_Data *data = (struct SDLGUI_Panel_Data*)element->data;

    SDLGUI_List_Destroy( data->elements, 1);
    SDL_DestroyTexture( data->texture);
    free( data);
    free( element);
}

struct SDLGUI_Element* SDLGUI_MouseDownHandler_Panel( struct SDLGUI_Element *panel, SDL_MouseButtonEvent *e) {
	e->x -= panel->rect.x;
	e->y -= panel->rect.y;
	struct SDLGUI_Panel_Data *data = (struct SDLGUI_Panel_Data*)panel->data;
	struct SDLGUI_Element *result = SDLGUI_Handle_MouseDown_List( data->elements, e);
	return result != 0 ? result : panel;
}

struct SDLGUI_Element* SDLGUI_Create_Panel( int xPos, int yPos, int width, int height, int bgColor[4], int borderColor[4], int borderThickness) {
    
    struct SDLGUI_Panel_Data *panelData = (struct SDLGUI_Panel_Data*)malloc( sizeof( struct SDLGUI_Panel_Data));
    panelData->texture = createElementTexture( width, height, bgColor, borderColor, borderThickness, 0, 0, 0);
    panelData->elements = SDLGUI_List_Create( 4);

    return SDLGUI_Create_Element( xPos, yPos, width, height, panelData, 0, &SDLGUI_Destroy_Panel, &SDLGUI_Draw_Panel, &SDLGUI_MouseDownHandler_Panel, 0);
}

void SDLGUI_Set_Panel_Elements( struct SDLGUI_Element *panel, struct SDLGUI_List *list, int destroy) {
	
	struct SDLGUI_Panel_Data *data = (struct SDLGUI_Panel_Data*)panel->data;
	if( destroy)
		SDLGUI_List_Destroy( data->elements, 1);
	data->elements = list;
}

/* Message Box
*/

void SDLGUI_Show_Message( int xPos, int yPos, int width, int height, enum SDLGUI_Message_Type msgType, const char *text) {
	int color[4] = {0,0,0,192};

	switch(msgType) {
		case SDLGUI_MESSAGE_INFO:
			color[0] = 0;
			color[1] = 100;
			color[2] = 0;
			break;
		case SDLGUI_MESSAGE_WARNING:
			color[0] = 100;
			color[1] = 100;
			color[2] = 0;
			break;
		case SDLGUI_MESSAGE_ERROR:
			color[0] = 100;
			color[1] = 0;
			color[2] = 0;
			break;
	};

	if( guiCore.messageBox) {
		SDLGUI_Destroy_Element( guiCore.messageBox);
	}
		
	guiCore.messageBox = SDLGUI_Create_Text ( xPos, yPos, width, height, NULL, text, color, (int[4]){255,255,255,255}, 18, 30, 5, NULL);
}

void SDLGUI_Hide_Message() {
	if( guiCore.messageBox) {
		SDLGUI_Destroy_Element( guiCore.messageBox);
		guiCore.messageBox = 0;
	}
}

/* Tooltip
*/

void SDLGUI_Destroy_Tooltip( struct SDLGUI_Element *tooltip) {
	SDLGUI_Destroy_Element( guiCore.tooltip);
	guiCore.tooltip = 0;
}

Uint32 mycallback(Uint32 interval, void *param) {
	if( guiCore.tooltip) {
		SDL_Event event;
		SDL_UserEvent userevent;
		
		
		userevent.type = SDL_USEREVENT;
		userevent.code = 0;
		userevent.data1 = &SDLGUI_Destroy_Tooltip;
		userevent.data2 = guiCore.tooltip;
		
		event.type = SDL_USEREVENT;
		event.user = userevent;
		
		SDL_PushEvent(&event);
	}
	return 0;
}

void SDLGUI_Show_Tooltip( int xPos, int yPos, const char *text) {
	int tooltipWidth, tooltipHeight;
	SDL_Texture *tooltipTexture = getTextTexture( guiCore.renderer, guiCore.font, text, 12, 16, (int[4]){255,255,255,128}, 0,0,0, &tooltipWidth, &tooltipHeight);

	if( xPos>tooltipWidth)
		xPos -= tooltipWidth;

	if( yPos>tooltipHeight)
		yPos -= tooltipHeight;

	if( guiCore.tooltip) {
		SDLGUI_Destroy_Element( guiCore.tooltip);
		SDL_RemoveTimer( guiCore.tooltipTimer);
	}
	guiCore.tooltip = SDLGUI_Create_Texture( xPos, yPos, tooltipWidth, tooltipHeight, tooltipTexture, &SDLGUI_Destroy_Tooltip, NULL);
	guiCore.tooltipTimer = SDL_AddTimer(1000, mycallback, 0);
}

/* Textbox
*/

struct SDLGUI_Text_Data {
	SDL_Texture *bg, *fg, *bgFocused, *fgFocused;
	char *text;
	int maxLen;
	int textWidth, textHeight;
	int fontWidth, fontHeight;
	int textColor[4];
	SDLGUI_TextChanged *textChanged;
	int acceptsNumbers, acceptsAlphabet;
};

void SDLGUI_Destroy_Textbox( struct SDLGUI_Element *textbox) {
	struct SDLGUI_Text_Data *data = (struct SDLGUI_Text_Data*)textbox->data;
	SDL_DestroyTexture( data->bg);
	SDL_DestroyTexture( data->fg);
	SDL_DestroyTexture( data->bgFocused);
	SDL_DestroyTexture( data->fgFocused);
	free( data->text);
	free( data);
	free( textbox);
}

void SDLGUI_Draw_Textbox( struct SDLGUI_Element *textbox, int x0, int y0) {
	if( ! textbox->isVisible)
		return;
	struct SDLGUI_Text_Data *data = (struct SDLGUI_Text_Data*)textbox->data;

	SDL_Rect rect = { textbox->rect.x + x0, textbox->rect.y + y0, textbox->rect.w, textbox->rect.h};

    int isFocusedElement = textbox == guiCore.focusedElement;

    SDL_RenderCopy( guiCore.renderer, isFocusedElement ? data->bgFocused : data->bg, NULL, &rect);
	rect.x += data->fontWidth/2;
	rect.y += data->fontHeight/2;
	rect.w = data->textWidth;
	rect.h = data->textHeight;
	SDL_RenderCopy( guiCore.renderer, isFocusedElement ? data->fgFocused : data->fg, NULL, &rect);

}

void SDLGUI_Focused_Textbox( struct SDLGUI_Element *textbox) {
	log0("focusing\n");
	guiCore.focusedElement = textbox;
}

struct SDLGUI_Element* SDLGUI_Create_Textbox( int xPos, int yPos, int maxLen, int allowedChars, int bgColor[4], int textColor[4], int fontWidth, int fontHeight, SDLGUI_TextChanged *textChanged) {
	if( maxLen >= 64)
		return 0;
	struct SDLGUI_Text_Data *data = (struct SDLGUI_Text_Data*)malloc( sizeof( struct SDLGUI_Text_Data));

	int width = fontWidth*(maxLen+1);
	int height = fontHeight*2;
	data->bg = createElementTexture( width, height, bgColor, textColor, 1, NULL, 0, 0);
	data->bgFocused = createElementTexture( width, height, (int[4]){textColor[0], textColor[1], textColor[2], 255}, textColor, 1, NULL, 0, 0);
	data->fontWidth = fontWidth;
	data->fontHeight = fontHeight;
	data->maxLen = maxLen;
	data->textColor[0] = textColor[0];
	data->textColor[1] = textColor[1];
	data->textColor[2] = textColor[2];
	data->textColor[3] = textColor[3];
	data->text = (char*)calloc( maxLen+1, sizeof(char));
	data->fg = getTextTexture( guiCore.renderer, guiCore.font, data->text, data->fontWidth, data->fontHeight, (int[4]){0,0,0,0}, data->textColor[0], data->textColor[1], data->textColor[2], &data->textWidth, &data->textHeight);
	data->fgFocused = getTextTexture( guiCore.renderer, guiCore.font, data->text, data->fontWidth, data->fontHeight, (int[4]){0,0,0,0}, 255-data->textColor[0], 255-data->textColor[1], 255-data->textColor[2], &data->textWidth, &data->textHeight);
	data->textChanged = textChanged;
	data->acceptsAlphabet = allowedChars & TEXTBOX_INPUT_ALPHABET;
	data->acceptsNumbers = allowedChars & TEXTBOX_INPUT_NUMERIC;

	return SDLGUI_Create_Element( xPos, yPos, width, height, data, &SDLGUI_Focused_Textbox, &SDLGUI_Destroy_Textbox, &SDLGUI_Draw_Textbox, NULL, 0);
}

void SDLGUI_SetText_Textbox( struct SDLGUI_Element *textbox, char *text) {
	struct SDLGUI_Text_Data *data = (struct SDLGUI_Text_Data*)textbox->data;
	if( text && strlen(text) > data->maxLen) 
		return; //TODO throw an error instead

	sprintf( data->text, "%s", text);

	if( data->fg) {
		SDL_DestroyTexture( data->fg);
		SDL_DestroyTexture( data->fgFocused);
	}

	data->fg = getTextTexture( guiCore.renderer, guiCore.font, data->text, data->fontWidth, data->fontHeight, (int[4]){0,0,0,0}, data->textColor[0], data->textColor[1], data->textColor[2], &data->textWidth, &data->textHeight);
	data->fgFocused = getTextTexture( guiCore.renderer, guiCore.font, data->text, data->fontWidth, data->fontHeight, (int[4]){0,0,0,0}, 255-data->textColor[0], 255-data->textColor[1], 255-data->textColor[2], &data->textWidth, &data->textHeight);
}
			
void SDLGUI_ChangeText_Textbox( struct SDLGUI_Element *textbox, char c, int backspace) {
	struct SDLGUI_Text_Data *data = (struct SDLGUI_Text_Data*)textbox->data;

	int curLen = strlen(data->text);
	int changed = 0;

	if( backspace && (data->acceptsNumbers || data->acceptsAlphabet)) {
		if( curLen > 0) {
			data->text[ curLen-1] = '\0';
			changed = 1;
		}
	}
	else if ( (data->acceptsNumbers && c >= '0' && c <= '9') || (data->acceptsAlphabet && c >= 'a' && c <= 'z') ){
		if( curLen < data->maxLen) {
			data->text[ curLen] = c;
			data->text[ curLen + 1] = '\0';
			changed = 1;
		}
	}

	if( changed) {
		SDL_DestroyTexture( data->fg);
		data->fg = getTextTexture( guiCore.renderer, guiCore.font, data->text, data->fontWidth, data->fontHeight, (int[4]){0,0,0,0}, data->textColor[0], data->textColor[1], data->textColor[2], &data->textWidth, &data->textHeight);
		data->fgFocused = getTextTexture( guiCore.renderer, guiCore.font, data->text, data->fontWidth, data->fontHeight, (int[4]){0,0,0,0}, 255-data->textColor[0], 255-data->textColor[1], 255-data->textColor[2], &data->textWidth, &data->textHeight);

		if( data->textChanged)
			data->textChanged( textbox, data->text);
	}
}

void SDLGUI_ClearText_Textbox( struct SDLGUI_Element *textbox) {
	struct SDLGUI_Text_Data *data = (struct SDLGUI_Text_Data*)textbox->data;
	if( data->text) {
		SDL_DestroyTexture( data->fg);
		sprintf( data->text, "%s", ""); //Writing it this way is necessary to avoid the zero-length warning
		data->fg = getTextTexture( guiCore.renderer, guiCore.font, data->text, data->fontWidth, data->fontHeight, (int[4]){0,0,0,0}, data->textColor[0], data->textColor[1], data->textColor[2], &data->textWidth, &data->textHeight);
		data->fgFocused = getTextTexture( guiCore.renderer, guiCore.font, data->text, data->fontWidth, data->fontHeight, (int[4]){0,0,0,0}, 255-data->textColor[0], 255-data->textColor[1], 255-data->textColor[2], &data->textWidth, &data->textHeight);
	}
}

const char* SDLGUI_GetText_Textbox( struct SDLGUI_Element *textbox) {
	struct SDLGUI_Text_Data *data = (struct SDLGUI_Text_Data*)textbox->data;
	return data->text;
}
