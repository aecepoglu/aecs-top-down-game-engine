#include <SDL2/SDL.h>
#include <SDL2/SDL_version.h>

#include <stdlib.h>
#include <assert.h>

SDL_version linked;
SDL_Window *win;
int running;
SDL_Renderer *ren;

void quit(char *msg) {
	fprintf(stderr, "Error msg: \"%s\"\n", msg);
	SDL_DestroyRenderer( ren);
	SDL_DestroyWindow( win);
	SDL_Quit();
	exit(0);
}

void init() {
	assert( SDL_Init(SDL_INIT_EVERYTHING) >= 0);
	win = SDL_CreateWindow("SDL Window", 0, 0, 640, 480, 0);
	assert( win);
}

/* Loads texture at given 'path' and returns it
*/
SDL_Texture *loadTexture( const char *path){
	SDL_Surface *img = SDL_LoadBMP( path);
	assert( img);
	SDL_Texture *tex = SDL_CreateTextureFromSurface( ren, img);
	assert( tex);
	SDL_FreeSurface( img);

	return tex;
}
/* Draws given texture 'tex' inside the given rectangle
*/
void drawTexture( SDL_Texture *tex, int x, int y, int w, int h) {
	SDL_Rect r = (SDL_Rect){x, y, w, h };
	SDL_RenderCopy( ren, tex, 0, &r);
}

int run() {
	SDL_Event e;
	ren = SDL_CreateRenderer( win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert( ren);

	SDL_Texture *tex = loadTexture( "res/img1.bmp");

	SDL_RenderClear( ren);
	drawTexture( tex, 50, 50, 32, 32);
	SDL_RenderPresent( ren);

	while( 1) {
		while( SDL_PollEvent( &e)) {
			if( e.type == SDL_QUIT)
				quit("Quitting");
			else
				printf("e type : %d\n", e.type);
		}
	}
}

int main() {
	SDL_VERSION( &linked);

	printf("sdl-version : %d.%d.%d\n", linked.major, linked.minor, linked.patch);

	init();
	running = 1;

	run();
}
