/* engine.h

	Common engine utilities used by both the game-application and the level-editor.

--------------------------------------
*/

#ifndef ENGINE_H
#define ENGINE_H

#include <SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#include "object.h"
#include "texture.h"
#include "map.h"
#include "../util/log.h"

/* ----------------------------------
 *  		VARIABLES
 */
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern struct GameTextures *textures;
extern SDL_Texture *bgroundTexture;

extern struct Map *myMap;

extern struct Vector viewPos, viewSize, viewEnd;
extern int windowW, windowH;

/* ---------------------------------
 *			DEFINITIONS
 */
#define TILELEN 32




/* --------------------------------
 *			FUNCTIONS
 */

/* Print error message and terminate application
*/
void quit( const char *);

/* Initializes SDL Windows and what-not
*/
void init( int offsetX, int offsetY, int winWidth, int winHeight);

/* Creates the background texture
*/
void drawBackground();

/* Draws the current view onto screen
*/
void draw();

/* Moves the view around
*/
bool scrollScreen( enum direction);

/* Resizez current viewPort to match the newly resized window
*/
void resizeView( int posX, int posY, int width, int height);

#endif //ENGINE_H
