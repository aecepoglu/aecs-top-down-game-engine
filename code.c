#include <SDL.h>
#include <SDL_version.h>
#include <stdlib.h>

SDL_version linked;
SDL_Surface *display;
int running;

void quit(char *msg) {
	fprintf(stderr, "Error msg: \"%s\"\n", msg);
	SDL_Quit();
	//exit(0);
}

void init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) 
		quit("coludn't init");
	if ( (display = SDL_SetVideoMode( 640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == 0)
		quit("couldn't set video");
}

int run() {
	SDL_Event e;
	while( running ) {
		while( SDL_PollEvent( &e)) {
			if( e.type == SDL_QUIT)
				running = 0;
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
