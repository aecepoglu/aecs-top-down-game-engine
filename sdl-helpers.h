#include <SDL.h>


/* Loads texture at given 'path' and returns it
*/
SDL_Texture *loadTexture( SDL_Renderer *ren, const char *path);

/* Draws given texture 'tex' inside the given rectangle
*/
void drawTexture( SDL_Renderer *ren, SDL_Texture *tex, int x, int y, int w, int h);
