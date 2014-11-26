#include "cutscene.h"

#define CUTSCENE_INIT_SIZE 8

void cutscene_init() {
	cutscene.elements.count = 0;
	cutscene.elements.size = CUTSCENE_INIT_SIZE;
	cutscene.elements.list = (struct CutsceneElement**)calloc( CUTSCENE_INIT_SIZE, sizeof( struct CutsceneElement*));
}

void cutscene_clear() {
	int i;
	for( i=0; i<cutscene.elements.count; i++) {
		SDL_DestroyTexture( cutscene.elements.list[i]->texture);
		free( cutscene.elements.list[i]);
		cutscene.elements.list[i] = NULL;
	}

	cutscene.elements.count = 0;
}

Uint32 cutscene_timer_callback( Uint32 interval, void *param) {
	SDL_Event event = {
		.type = SDL_USEREVENT,
	};

	SDL_PushEvent( &event);

	return 0;
}

void cutscene_wait( SDL_Renderer *renderer, int miliseconds) {
	SDL_AddTimer( miliseconds, cutscene_timer_callback, NULL);

	int cutsceneRunning = 1;

	SDL_Event e;
	while( cutsceneRunning) {
		SDL_WaitEvent( &e);
		switch(e.type) {
			case SDL_USEREVENT: 
				cutsceneRunning = 0;
				break;
			case SDL_WINDOWEVENT:
				cutscene_draw( renderer);
				break;
		}
	}
}

void cutscene_draw( SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor( renderer, cutscene.bgColor.r, cutscene.bgColor.g, cutscene.bgColor.b, 255);
	SDL_RenderClear( renderer);

	struct CutsceneElement *elem;
	int i;
	for( i=0; i<cutscene.elements.count; i++) {
		elem = cutscene.elements.list[ i];
		SDL_RenderCopy( renderer, elem->texture, NULL, &elem->rect);
	}

	SDL_RenderPresent( renderer);
}
