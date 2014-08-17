#ifndef TEXT_H
#define TEXT_H

#include "texture.h"

void drawText( SDL_Renderer *ren, SDL_Texture **font, const char *text, int xPos, int yPos, int unitWidth, int unitHeight);

SDL_Texture* getTextTexture( SDL_Renderer *ren, SDL_Texture **font, const char *text, int charWidth, int charHeight, int bgColor[4], int R, int G, int B, int *width, int *height);

#endif /*TEXT_H*/
