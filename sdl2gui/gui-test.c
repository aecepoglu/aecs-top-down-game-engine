#include <SDL.h>
#include <stdlib.h>
#include <assert.h>

#include "../log.h"
#include "../basic.h"
#include "../texture.h"
#include "../text.h"

#include "sdl2gui.h"

SDL_Window *window;
SDL_Renderer *renderer;
bool running;
struct GameTextures *textures;


//SDL_Texture *coloredText;
//int width, height;

void quit( const char *msg) {
	//TODO handle the termination properly.
	fprintf(stderr, "Error msg: \"%s\"\n", msg);
	SDL_DestroyRenderer( renderer);
	SDL_DestroyWindow( window);
	SDL_Quit();
	exit(0);
}

void handleKey( SDL_KeyboardEvent *e) {
	switch (e->keysym.sym) {
		case SDLK_s:
			/* saves the map, and continues */
			log0("s key pressed\n");
			break;
		case SDLK_q:
			/* terminates program without saving anything */
			running =0;
			break;

		case SDLK_UP:
			log0("up key\n");
			break;
		case SDLK_DOWN:
			log0("down key\n");
			break;
		case SDLK_RIGHT:
			log0("right key\n");
			break;
		case SDLK_LEFT:
			log0("left key\n");
			break;
		default:
			if( e->keysym.sym >= SDLK_0 && e->keysym.sym <= SDLK_9)
				log0("numeric key\n");
			break;
	};
}

/* The mouse button-event and motion-events are handled the same
 * I just need to extract the x,y from the event-data.
 */
bool handleMouse( SDL_MouseButtonEvent *e, SDL_MouseMotionEvent *e2) {
	return true;
}

void draw() {
	log3("draw\n");
	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0);
	SDL_RenderClear( renderer);

	char *text1 = "quick brown fox jumps over the lazy dog!";
	drawText( renderer, textures->font, text1, 10, 10, 6, 8);
	drawText( renderer, textures->font, text1, 10, 30, 12, 16);
	drawText( renderer, textures->font, text1, 10, 70, 18, 24);

	//drawTexture( renderer, coloredText, 150, 150, width, height);

	SDLGUI_Draw();



	SDL_RenderPresent( renderer);
}


void run() {
	draw();
	SDL_Event e;
	while( running) {
		//while( SDL_PollEvent( &e)) {
		//}
		SDL_WaitEvent( &e);
		switch (e.type) {
			case SDL_WINDOWEVENT:
				switch( e.window.event) {
					case SDL_WINDOWEVENT_SHOWN:
					    log1("Window %d shown\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_HIDDEN:
					    log1("Window %d hidden\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_EXPOSED:
					    //log1("Window %d exposed\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_RESIZED:
					    log1("Window %d resized to %dx%d\n",
					            e.window.windowID, e.window.data1,
					            e.window.data2);
					    break;
					case SDL_WINDOWEVENT_MINIMIZED:
					    log1("Window %d minimized\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_MAXIMIZED:
					    log1("Window %d maximized\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_RESTORED:
					    log1("Window %d restored\n", e.window.windowID);
					    break;
					case SDL_WINDOWEVENT_CLOSE:
					    log1("Window %d closed\n", e.window.windowID);
					    break;
					default:
						//unhandled window event
						continue;
				};
				break;
			case SDL_QUIT:
				quit("Quitting");
				running =0;
				break;
			case SDL_KEYDOWN:
				log1("key down\n");
				handleKey( (SDL_KeyboardEvent*)&e);
				break;
			case SDL_MOUSEBUTTONDOWN:
				log0("mouse button down\n");
				break;
			case SDL_MOUSEMOTION:
				log0("mouse motion\n");
				break;
			case SDL_KEYUP:
			case SDL_MOUSEBUTTONUP:
				/*don't do anything for those events*/
				continue;
			default:
				log1("unhandled event type: %d\n", e.type);
				continue;
		};
		draw();
	}
}


void init() {
	assert( SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_TIMER) >= 0);
	window = SDL_CreateWindow("sdl-window", 0, 0, 800, 400, 0);
	assert( window);
	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert( renderer);
}

void myButton_clicked( struct SDLGUI_Element *from) {
	log0("my button clicked\n");
}

int main( int argc, char *args[]) {
	init();

	textures = loadAllTextures( renderer);
	SDLGUI_Init( renderer, textures->font);
	int color[4] = {255,0,0,0};
	struct SDLGUI_Element *myButton = SDLGUI_Create_Text( 10, 20, 200, 100, &myButton_clicked, "my button", (int[4]){255,0,0,0}, color);
	SDLGUI_Add_Element( myButton);


	log0("All set and ready\nStarting...\n");
	//coloredText = getTextTexture( "red", 12, 20, 255, 0, 0, &width, &height);

	running = 1;
	run();

	log0("Program over\nPeace\n");
	SDLGUI_Destroy();
	log0("gui destroyed\n");
	freeTextures( textures);
	quit("Willingly quitting\n");
	//SDL_DestroyRenderer( renderer);
	//log0("renderer destroyed\n");
	//SDL_DestroyWindow( window);
	//log0("window destroyed\n");
	//SDL_Quit();
	log0("returning 0 and out\n");
	return 0;
}
