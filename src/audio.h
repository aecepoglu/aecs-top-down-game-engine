#ifndef AUDIO_H
#define AUDIO_H

#include "SDL_mixer.h"

enum AudioFxType {
	AUDIO_MOVE,
	AUDIO_INTERACT,
	AUDIO_CONSOLE,

	AUDIO_NUM_ITEMS
};

struct {
	Mix_Chunk *fx[ AUDIO_NUM_ITEMS];
} audios;



#define PLAY_AUDIO_FX( type) Mix_PlayChannel( -1, audios.fx[ type], 0);

void audio_init();

void audio_free();


#endif /*AUDIO_H*/
