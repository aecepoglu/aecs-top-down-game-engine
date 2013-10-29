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
#include "log.h"

/* ----------------------------------
 *  		VARIABLES 
 */
bool running;
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture **textures;
SDL_Texture *bgroundTexture;

struct Map *myMap;

/* windowX,windowY : coordinates of current view in the whole map */
int windowW, windowH, windowX, windowY;

/* ---------------------------------
 *			DEFINITIONS
 */
#define TILELEN 16




/* --------------------------------
 *			FUNCTIONS
 */

/* Print error message and terminate application
*/
void quit( const char *);

/* Initializes SDL Windows and what-not
*/
void init();

/* Draws objects over the renderere
*/
void drawObjects();

/* Creates the background texture
*/
void drawBackground();

/* Draws the current view onto screen
*/
void draw();

/* Moves the view around
*/
void scrollScreen( int, int);

#endif //ENGINE_H
