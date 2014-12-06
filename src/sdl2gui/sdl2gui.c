#include <string.h>

#include "sdl2gui.h"
#include "sdl2gui-text.h"
#include "../text.h"
#include "../log.h"


void SDLGUI_Init( SDL_Renderer *renderer, SDL_Texture **font) {
	guiCore.renderer = renderer;
	guiCore.font = font;
	guiCore.mouseDownElement = 0;
	guiCore.mouseHoverElement = 0;
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
		guiCore.elements.list[i]->drawFun( guiCore.elements.list[i]);
	}

	if( guiCore.messageBox)
		guiCore.messageBox->drawFun( guiCore.messageBox);

	if( guiCore.tooltip)
		guiCore.tooltip->drawFun( guiCore.tooltip);
}

int SDLGUI_Handle_MouseDown( SDL_MouseButtonEvent *e) {
	struct SDLGUI_Element *mouseDownElement = SDLGUI_List_findItemAtPos( &guiCore.elements, e->x, e->y);
	if( mouseDownElement != 0) {
		guiCore.mouseDownElement = mouseDownElement;
		return 1;
	}
	else
		return 0;
}

int SDLGUI_Handle_MouseHover( SDL_MouseMotionEvent *e) {
	if( ! e->state ) {
		struct SDLGUI_Element *hoverElement = SDLGUI_List_findItemAtPos( &guiCore.elements, e->x, e->y);
		if( hoverElement != 0 && guiCore.mouseHoverElement != hoverElement) {
			if( guiCore.mouseHoverElement != 0) {
				guiCore.mouseHoverElement->textures.current = (guiCore.mouseHoverElement == guiCore.focusedElement)
					? guiCore.mouseHoverElement->textures.focused
					: guiCore.mouseHoverElement->textures.normal
				;
			}

			guiCore.mouseHoverElement = hoverElement;
			
			if( hoverElement->textures.hover != 0 && hoverElement->clicked != 0)
				hoverElement->textures.current = hoverElement->textures.hover;

			return 1;
		}
	}
	return 0;
}

void SDLGUI_Handle_MouseUp( SDL_MouseButtonEvent *e) {
	if( guiCore.focusedElement != 0) {
		guiCore.focusedElement->textures.current = guiCore.focusedElement->textures.normal;
	}
	if( guiCore.mouseDownElement && guiCore.mouseDownElement->clicked != 0) {
		guiCore.mouseDownElement->clicked( guiCore.mouseDownElement);
		guiCore.focusedElement = guiCore.mouseDownElement;
		guiCore.focusedElement->textures.current = guiCore.focusedElement->textures.focused;
		guiCore.mouseDownElement = 0;
	}
	else {
		guiCore.focusedElement = 0;
	}
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
			log1("input: %c, backspace: %d\n", c, backspace);
			SDLGUI_ChangeText_Textbox( guiCore.focusedElement, c, backspace); //TODO don't forget to undo this
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

/* Message Box
*/

void SDLGUI_Show_Message( int xPos, int yPos, int width, int height, enum SDLGUI_Message_Type msgType, const char *text) {
	SDLGUI_Color color = {.r=0, .g=0, .b=0, .a=192};

	switch(msgType) {
		case SDLGUI_MESSAGE_INFO:
			color.g = 100;
			break;
		case SDLGUI_MESSAGE_WARNING:
			color.r = 100;
			color.g = 100;
			break;
		case SDLGUI_MESSAGE_ERROR:
			color.r = 100;
			break;
	};

	if( guiCore.messageBox) {
		SDLGUI_Destroy_Element( guiCore.messageBox);
	}
		
	guiCore.messageBox = SDLGUI_Create_Text ( (SDL_Rect){.x=xPos, .y=yPos, .w=width, .h=height}, text, (SDLGUI_Params){.bgColor=color, .fgColor=COLOR_WHITE, .fontWidth=18, .fontHeight= 30, .borderThickness=5 });
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
	guiCore.tooltip = (struct SDLGUI_Element*)malloc( sizeof( struct SDLGUI_Element));
	guiCore.tooltip->rect = (SDL_Rect){
		.x = xPos,
		.y = yPos,
		.w = tooltipWidth,
		.h = tooltipHeight
	};
	guiCore.tooltip->destructor = SDLGUI_Destroy_Texture;
	guiCore.tooltip->drawFun = SDLGUI_Draw_Texture;
	guiCore.tooltip->textures.current = tooltipTexture;
	guiCore.tooltipTimer = SDL_AddTimer(1000, mycallback, 0);
}
