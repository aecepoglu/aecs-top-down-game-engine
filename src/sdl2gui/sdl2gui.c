#include <string.h>
#include <stdlib.h>

#include "sdl2gui.h"
#include "sdl2gui-text.h"
#include "../text.h"
#include "../util/log.h"

struct SDLGUI_Layer {
	struct SDLGUI_List *list;
	struct SDLGUI_Layer *prev;
};

struct SDLGUI_Layer *topLayer = 0;


void SDLGUI_Init( SDL_Window *window, SDL_Renderer *renderer, SDL_Texture **font) {
	guiCore.window = window;
	guiCore.renderer = renderer;
	guiCore.font = font;
	guiCore.mouseDownElement = 0;
	guiCore.mouseHoverElement = 0;
	guiCore.tooltip = 0;
	guiCore.focusedElement = 0;
	topLayer = 0;
}

void SDLGUI_Destroy() {
	while(topLayer != 0) {
		SDLGUI_Layer_Remove( topLayer);
	}

	guiCore.renderer = 0;
	guiCore.font = 0;
}

void SDLGUI_Draw() {
	if (topLayer == 0)
		return;

	struct SDLGUI_List *elements = topLayer->list;
	int i;
	for( i=0; i<elements->count; i++) {
		elements->list[i]->drawFun( elements->list[i]);
	}

	if( guiCore.messageBox)
		guiCore.messageBox->drawFun( guiCore.messageBox);

	if( guiCore.tooltip)
		guiCore.tooltip->drawFun( guiCore.tooltip);
}

int SDLGUI_Handle_MouseDown( SDL_MouseButtonEvent *e) {
	if( topLayer == 0)
		return 0;
	else if( guiCore.messageBox) {
		SDLGUI_Hide_Message();
		return 1;
	}

	struct SDLGUI_Element *mouseDownElement = SDLGUI_List_findItemAtPos( topLayer->list, e->x, e->y);
	if( mouseDownElement != 0) {
		guiCore.mouseDownElement = mouseDownElement;
		return 1;
	}
	else
		return 0;
}

int SDLGUI_Handle_MouseHover( SDL_MouseMotionEvent *e) {
	if( ! e->state && topLayer != 0 && guiCore.messageBox == 0) {
		struct SDLGUI_Element *hoverElement = SDLGUI_List_findItemAtPos( topLayer->list, e->x, e->y);
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
	if( guiCore.focusedElement && guiCore.focusedElement->textInputHandler) {
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
			guiCore.focusedElement->textInputHandler( guiCore.focusedElement, c, backspace);
			return 1;
		}
	}
	return 0;
};


/* Message Box
*/


void SDLGUI_Show_Message( enum SDLGUI_Message_Type msgType, const char *text) {
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

	SDLGUI_Hide_Message();

	int width, height;
	SDL_GetWindowSize( guiCore.window, &width, &height);

	guiCore.messageBox = SDLGUI_Create_Text ( (SDL_Rect){.x=0, .y=0, .w=width, .h=height}, text, (SDLGUI_Params){.bgColor=color, .fgColor=COLOR_WHITE, .fontWidth=18, .fontHeight= 30, .borderThickness=5 });
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
	guiCore.tooltip->isVisible = 1;
	guiCore.tooltip->destructor = SDLGUI_Destroy_Texture;
	guiCore.tooltip->drawFun = SDLGUI_Draw_Texture;
	guiCore.tooltip->textures.normal = guiCore.tooltip->textures.current = tooltipTexture;
	guiCore.tooltip->textures.focused = guiCore.tooltip->textures.hover = 0;

	guiCore.tooltipTimer = SDL_AddTimer(1000, mycallback, 0);
}

/* Layer
*/

void SDLGUI_Layer_Add( struct SDLGUI_List *list) {
	struct SDLGUI_Layer *layer = (struct SDLGUI_Layer*)malloc(sizeof(struct SDLGUI_Layer));

	layer->prev = topLayer;
	layer->list = list;

	topLayer = layer;
}

void SDLGUI_Layer_Remove() {
	struct SDLGUI_Layer *prev = topLayer->prev;

	SDLGUI_List_Destroy( topLayer->list);
	free(topLayer);

	topLayer = prev;
}
