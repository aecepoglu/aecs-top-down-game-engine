#ifndef FILE_PICKER_H
#define FILE_PICKER_H

#include "../sdl2gui.h"

void SDLGUI_OpenFilePicker(const char *initialDir, const char *wantedFilename, void (*successCallback)(const char *path), void (*cancelCallback)());

#endif /*FILE_PICKER_H*/
