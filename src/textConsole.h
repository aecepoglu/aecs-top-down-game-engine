#ifndef TEXTCONSOLE_H
#define TEXTCONSOLE_H

#include "text.h"

#define CONSOLE_NUM_COLS 40
#define CONSOLE_NUM_COLS_PLUS_1 41
#define CONSOLE_NUM_ROWS 5

#define CONSOLE_FONT_WIDTH 12
#define CONSOLE_FONT_HEIGHT 16

#define CONSOLE_WIDTH 480 /* CONSOLE_NUM_COLS * CONSOLE_FONT_WIDTH */
#define CONSOLE_HEIGHT 80 /* CONSOLE_NUM_ROWS * CONSOLE_FONT_HEIGHT */

void textConsole_write( SDL_Renderer *renderer, SDL_Texture **fontMap, SDL_Texture *consoleTexture);
void textConsole_add( const char *text);
SDL_Texture* textConsole_init( SDL_Renderer *renderer);

#endif /*TEXTCONSOLE_H*/
