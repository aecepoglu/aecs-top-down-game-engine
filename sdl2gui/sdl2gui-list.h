#include "sdl2gui.h"
#include "../array.h"

struct SDLGUI_List {
	struct SDLGUI_Element **list;
	int count;
	int size;
};

void SDLGUI_List_Add( struct SDLGUI_List *list, struct SDLGUI_Element *item);
void SDLGUI_List_Remove( struct SDLGUI_List *list, struct SDLGUI_Element *item);
void SDLGUI_List_Clear( struct SDLGUI_List *list, int freeItems);
void SDLGUI_List_Destroy( struct SDLGUI_List *list, int freeSelf);
void SDLGUI_List_Init( struct SDLGUI_List *list, int initSize);
