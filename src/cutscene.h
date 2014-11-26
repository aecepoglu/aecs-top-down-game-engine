#include <SDL.h>
#include "array.h"

struct CutsceneElement {
	SDL_Rect rect;
	SDL_Texture *texture;
};

struct {
	struct {
		int r, g, b;
	} bgColor;

	struct {
		struct CutsceneElement **list;
		int count;
		int size;
	} elements;
} cutscene;

/*
*/

void cutscene_init();
void cutscene_clear();

#define cutscene_add( item) ARRAY_ADD( cutscene.elements.list, item, cutscene.elements.count, cutscene.elements.size, sizeof( struct CutsceneElement*) )

void cutscene_wait( SDL_Renderer *renderer, int miliseconds);
void cutscene_draw( SDL_Renderer *renderer);
