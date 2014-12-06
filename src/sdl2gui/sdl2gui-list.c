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
	int newSize = list->count < list->size / 4
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
	list->size = newSize;
	list->count = newCount;
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

struct SDLGUI_List* SDLGUI_List_Create_From_Array( struct SDLGUI_Element **items, int count) {
	struct SDLGUI_List *list = SDLGUI_List_Create( count);
	
	list->count = count;

	int i;
	for( i=0; i<count; i++) {
		list->list[i] = items[i];
	}

	return list;
}

void SDLGUI_List_Init( struct SDLGUI_List *list, int initSize) {
	list->list = (struct SDLGUI_Element**)calloc( initSize, sizeof( struct SDLGUI_Element*));
	list->size = initSize;
	list->count = 0;
}

struct SDLGUI_Element* SDLGUI_List_findItemAtPos( struct SDLGUI_List *list, int x, int y) {
	struct SDLGUI_Element *elem;
	int i;
	for(i=0; i<list->count; i++) {
		elem = list->list[i];
		if (elem->isVisible && x >= elem->rect.x && y >= elem->rect.y && x<elem->rect.x+elem->rect.w && y<elem->rect.y+elem->rect.h) {
			if( elem->mouseHandler != 0)
				return elem->mouseHandler( elem, x, y);
			else
				return elem;
		}
	}
	return 0;
}
