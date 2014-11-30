#ifndef AUDIO_H
#define AUDIO_H

#include "SDL_mixer.h"

enum AudioFxType {
	AUDIO_NONE,

	AUDIO_MOVE,
	AUDIO_NO_MOVE,
	AUDIO_INTERACT,
	AUDIO_NO_INTERACT,
	AUDIO_PUSH,
	AUDIO_NO_PUSH,
	AUDIO_HIT,
	AUDIO_NO_HIT,
	AUDIO_EAT,
	AUDIO_NO_EAT,
	AUDIO_PICK,
	AUDIO_NO_PICK,
	AUDIO_DROP,
	AUDIO_NO_DROP,
	AUDIO_CONSOLE,

	AUDIO_NUM_ITEMS
};

struct {
	Mix_Chunk *fx[ AUDIO_NUM_ITEMS];
} audios;



#define PLAY_AUDIO_FX( type, distance) if( distance < HEARING_DISTANCE ) Mix_PlayChannel( distance, audios.fx[ type], 0);
#define HEARING_DISTANCE 16

void audio_init();

void audio_free();


#endif /*AUDIO_H*/
