#ifndef LEVEL_EDITOR_H
#define LEVEL_EDITOR_H

#include "../sdl2gui/sdl2gui.h"
#include "../core/engine.h"


extern struct SpriteSpecsList *spriteSpecs;


void buttonQuit_clicked( struct SDLGUI_Element *from);

void drawBackground();
void draw();

#endif /*LEVEL_EDITOR_H*/
