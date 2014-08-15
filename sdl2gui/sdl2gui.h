struct SDLGUI_Element {
    int id;
    int x, y;
    int width, height;

    void *data;

    void (clicked)( SDLGUI_Element *fromElement, SDL_Mouse_Event e);
    void (drawFun)(SDLGUI_Element *element);
};

void SDLGUI_Draw_Image( SDLGUI_Element *element);

#define SDLGUI_Create_Image( xPos, yPos, widthValue, heightValue, clickCallback) SDLGUI_Create_Element( xPos, yPos, widthValue, heightValue, clickCallback, SDLGUI_Draw_Image)

void button_load_clicked( SDLGUI_Element *from, SDL_Mouse_Event e) {
    log0("load-button clicked")
}

struct SDLGUI_Element *imgElement = SDLGUI_Create_Image( 10, 10, 200, 100, button_load_clicked);

//mouse-down event is used for painting tiles etc on level
//mouse-up event is used for the 'click' event, used for buttons
