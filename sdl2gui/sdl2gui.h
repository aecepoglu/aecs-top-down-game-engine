#ifndef SDL2GUI_H
#define SDL2GUI_H

#include <SDL.h>

struct SDLGUI_Element;

typedef void (SDLGUI_Clicked)(struct SDLGUI_Element *);
typedef void (SDLGUI_Destructor)(struct SDLGUI_Element *);
typedef void (SDLGUI_DrawFun)(struct SDLGUI_Element *);

struct SDLGUI_Element {
	SDL_Rect rect;

    void *data;

	SDLGUI_Clicked *clicked;
	SDLGUI_Destructor *destructor;
	SDLGUI_DrawFun *drawFun;
};


void SDLGUI_Init( SDL_Renderer *renderer, SDL_Texture **font);
void SDLGUI_Add_Element( struct SDLGUI_Element *element);
void SDLGUI_Draw();
void SDLGUI_Destroy();
int SDLGUI_Handle_MouseDown( SDL_MouseButtonEvent *e);
void SDLGUI_Handle_MouseUp( SDL_MouseButtonEvent *e);


struct SDLGUI_Element* SDLGUI_Create_Element( int xPos, int yPos, int width, int height, void *data, SDLGUI_Clicked *clicked, SDLGUI_Destructor *dtor, SDLGUI_DrawFun *drawFun);
#define SDLGUI_Destroy_Element( element) (element)->destructor( element)

#define SDLGUI_Create_Texture( xPos, yPos, width, height, texture, clicked) SDLGUI_Create_Element( xPos, yPos, width, height, texture, clicked, SDLGUI_Destroy_Texture, SDLGUI_Draw_Texture)
void SDLGUI_Draw_Texture( struct SDLGUI_Element *element);
void SDLGUI_Destroy_Texture( struct SDLGUI_Element *element);

struct SDLGUI_Element* SDLGUI_Create_Text( int xPos, int yPos, int width, int height, SDLGUI_Clicked *clicked, const char *text, int bgColor[4], int fgColor[4]);

struct SDLGUI_Element* SDLGUI_Create_Image( int xPos, int yPos, int width, int height, SDLGUI_Clicked *clicked, const char *imgPath, int bgColor[4]);


/*

mouse-down event is used for painting tiles etc on level
mouse-up event is used for the 'click' event, used for buttons

*/

#endif /*SDL2GUI_H*/
