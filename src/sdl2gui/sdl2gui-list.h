#ifndef SDLGUI_LIST_H
#define SDLGUI_LIST_H

struct SDLGUI_List;

#include "sdl2gui-element.h"

struct SDLGUI_List {
	struct SDLGUI_Element **list;
	int count;
	int size;
};

void SDLGUI_List_Add( struct SDLGUI_List *list, struct SDLGUI_Element *item);
void SDLGUI_List_Remove( struct SDLGUI_List *list, struct SDLGUI_Element *item);
void SDLGUI_List_Clear( struct SDLGUI_List *list, int freeItems);
void SDLGUI_List_Destroy( struct SDLGUI_List *list);
struct SDLGUI_List* SDLGUI_List_Create( int initSize);
struct SDLGUI_List* SDLGUI_List_Create_From_Array( struct SDLGUI_Element **items, int count);
void SDLGUI_List_Init( struct SDLGUI_List *list, int initSize);

struct SDLGUI_Element* SDLGUI_List_findItemAtPos( struct SDLGUI_List *list, int x, int y);

#endif /*SDLGUI_LIST_H*/
