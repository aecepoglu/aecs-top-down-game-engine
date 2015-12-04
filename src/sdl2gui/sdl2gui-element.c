#include <stdlib.h>
#include "sdl2gui-element.h"
#include "sdl2gui.h"

void SDLGUI_Draw_Texture( struct SDLGUI_Element *elem) {
	if( elem->isVisible) {
		SDL_RenderCopy( guiCore.renderer, elem->textures.current, NULL, &elem->rect);
	}
}

void SDLGUI_Destroy_Texture( struct SDLGUI_Element *element) {
	SDL_DestroyTexture( element->textures.normal);

	if( element->textures.hover)
		SDL_DestroyTexture( element->textures.hover);

	if( element->textures.focused)
		SDL_DestroyTexture( element->textures.focused);

	free( element);
}

SDL_Texture *createElementTexture( int width, int height, SDLGUI_Color bgColor, SDLGUI_Color borderColor, int borderThickness, SDL_Texture *fgTexture, int fgWidth, int fgHeight, int horAlignment) {
	SDL_Texture *texture = SDL_CreateTexture( guiCore.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height) ;
	SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND);


	SDL_SetRenderTarget( guiCore.renderer, texture);

	SDL_SetRenderDrawColor( guiCore.renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
	SDL_RenderClear( guiCore.renderer);

	SDL_SetRenderDrawColor( guiCore.renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
	SDL_Rect rect = {.x=borderThickness, .y=borderThickness, .w=width-2*borderThickness, .h=height-2*borderThickness};
	SDL_RenderFillRect( guiCore.renderer, &rect);

    if( fgTexture) {
	    //TODO assert width > fgWidth, height>fgHeight
	    rect.x = horAlignment == ALIGN_LEFT ? 0 : (width - fgWidth) / 2;
	    rect.y = (height - fgHeight) / 2;
	    rect.w = fgWidth;
	    rect.h = fgHeight;
	    SDL_RenderCopy( guiCore.renderer, fgTexture, NULL, &rect);
    }

	SDL_SetRenderTarget( guiCore.renderer, 0);

	return texture;
}

void SDLGUI_Texture_SetTextures( struct SDLGUI_Element *e, /*const*/SDL_Texture *texture, int textureWidth, int textureHeight, SDLGUI_Params params) {
	e->textures.normal = e->textures.current = createElementTexture( e->rect.w, e->rect.h, params.bgColor, params.fgColor, params.borderThickness, texture, textureWidth, textureHeight, params.horizontalAlignment);

	e->textures.hover = createElementTexture( e->rect.w, e->rect.h, (SDLGUI_Color){
			.r = (params.fgColor.r + params.bgColor.r) / 2,
			.g = (params.fgColor.g + params.bgColor.g) / 2,
			.b = (params.fgColor.b + params.bgColor.b) / 2,
			.a = params.bgColor.a + (255 - params.bgColor.a) / 2
		}, params.fgColor, params.borderThickness, texture, textureWidth, textureHeight, params.horizontalAlignment);
	
	
	e->textures.focused = createElementTexture( e->rect.w, e->rect.h, (SDLGUI_Color){
			.r = (params.fgColor.r + 3*params.bgColor.r) / 4,
			.g = (params.fgColor.g + 3*params.bgColor.g) / 4,
			.b = (params.fgColor.b + 3*params.bgColor.b) / 4,
			.a = params.bgColor.a + (255 - params.bgColor.a) / 4
		}, params.fgColor, params.borderThickness +2, texture, textureWidth, textureHeight, params.horizontalAlignment);
}

//TODO the texture should be const
struct SDLGUI_Element *SDLGUI_Create_Texture( SDL_Rect rect, /*const*/SDL_Texture *texture, int textureWidth, int textureHeight, SDLGUI_Params params) {
	struct SDLGUI_Element *e = SDLGUI_CreateElement(rect);
	e->destructor = SDLGUI_Destroy_Texture;
	e->drawFun = SDLGUI_Draw_Texture;
	e->isVisible = !params.isHidden;
	e->scrollHandler = 0;

	SDLGUI_Texture_SetTextures( e, texture, textureWidth, textureHeight, params);

	return e;
}

struct SDLGUI_Element *SDLGUI_CreateElement(SDL_Rect rect) {
	struct SDLGUI_Element *e = (struct SDLGUI_Element*)malloc(sizeof(struct SDLGUI_Element));

	e->sizeHints = SDLGUI_SIZEHINTS_FIXED;
	if (rect.w == SDLGUI_SIZE_FILL || rect.h == SDLGUI_SIZE_FILL) {
		int winWidth, winHeight;

		SDL_GetWindowSize( guiCore.window, &winWidth, &winHeight);

		if (rect.w == SDLGUI_SIZE_FILL) {
			e->sizeHints = e->sizeHints | SDLGUI_SIZEHINTS_STRETCH_HORIZONTAL;
			rect.w = winWidth - rect.x;
		}
		if (rect.h == SDLGUI_SIZE_FILL) {
			e->sizeHints = e->sizeHints | SDLGUI_SIZEHINTS_STRETCH_VERTICAL;
			rect.h = winHeight - rect.y;
		}
	}

	e->rect = rect;

	e->clicked = 0;
	e->mouseDown = 0;
	e->mouseMotion = 0;
	e->mouseHandler = 0;
	e->resizeHandler = 0;
	e->textInputHandler = 0;
	e->scrollHandler = 0;

	return e;
}
