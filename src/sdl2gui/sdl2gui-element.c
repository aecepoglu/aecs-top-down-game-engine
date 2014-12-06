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

SDL_Texture *createElementTexture( int width, int height, SDLGUI_Color bgColor, SDLGUI_Color borderColor, int borderThickness, SDL_Texture *fgTexture, int fgWidth, int fgHeight) {
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
	    rect.x = (width - fgWidth) / 2;
	    rect.y = (height - fgHeight) / 2;
	    rect.w = fgWidth;
	    rect.h = fgHeight;
	    SDL_RenderCopy( guiCore.renderer, fgTexture, NULL, &rect);
    }

	SDL_SetRenderTarget( guiCore.renderer, 0);

	return texture;
}
