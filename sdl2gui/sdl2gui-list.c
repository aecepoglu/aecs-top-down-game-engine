#include <stdlib.h>

#include "sdl2gui-list.h"

void SDLGUI_List_Add( struct SDLGUI_List *list, struct SDLGUI_Element *item) {
	if ( list->count == list->size) {
		list->size *= 2;
		list->list = realloc( list->list, list->size * sizeof(struct SDLGUI_Element*));
	}
	list->list[ list->count] = item;
	list->count ++;
}

void SDLGUI_List_Remove( struct SDLGUI_List *list, struct SDLGUI_Element *item) {
	int newSize = list->count < list->size / 2
		? list->size/2
		: list->size;
	struct SDLGUI_Element **new = (struct SDLGUI_Element**)calloc( newSize, sizeof( struct SDLGUI_Element*));

	int newCount=0;

	int i=0;
	for( i=0; i<list->count; i++) {
		if( list->list[i] != item) {
			new[ newCount] = list->list[i];
			newCount ++;
		}
	}

	free( list->list);
	list->list = new;
}

void SDLGUI_List_Clear( struct SDLGUI_List *list, int freeItems) {
	if( freeItems) {
		int i;
		for( i=0; i<list->count; i++) {
			SDLGUI_Destroy_Element( list->list[i]);
		}
	}
	list->count = 0;
}

void SDLGUI_List_Destroy( struct SDLGUI_List *list, int freeSelf) {
	int i;
	for( i=0; i<list->count; i++) {
		SDLGUI_Destroy_Element( list->list[i]);
	}
	free( list->list);
	if( freeSelf)
		free( list);
}

struct SDLGUI_List* SDLGUI_List_Create( int initSize) {
	struct SDLGUI_List *x = (struct SDLGUI_List*)malloc( sizeof(struct SDLGUI_List));
	SDLGUI_List_Init( x, initSize);
	return x;
}

void SDLGUI_List_Init( struct SDLGUI_List *list, int initSize) {
	list->list = (struct SDLGUI_Element**)calloc( initSize, sizeof( struct SDLGUI_Element*));
	list->size = initSize;
	list->count = 0;
}
