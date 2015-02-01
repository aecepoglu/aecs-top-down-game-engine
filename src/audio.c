#include "audio.h"
#include "log.h"
#include <stdlib.h>

void audio_init() {
	if( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 2048) < 0) {
		printf( "SDL_mixer couldn't initialize. Error: %s\n", Mix_GetError());
	}

	char *fxPaths[AUDIO_NUM_ITEMS] = {
		[AUDIO_MOVE] 		= "res/audio/move.wav",
		[AUDIO_NO_MOVE] 	= "res/audio/noAction.wav",
		[AUDIO_INTERACT] 	= "res/audio/interact.wav",
		[AUDIO_NO_INTERACT] = "res/audio/noAction.wav",
		[AUDIO_PUSH] 		= "res/audio/push.wav",
		[AUDIO_NO_PUSH] 	= "res/audio/noAction.wav",
		[AUDIO_HIT]			= "res/audio/hit.wav",
		[AUDIO_NO_HIT]		= "res/audio/noAction.wav",
		[AUDIO_EAT]			= "res/audio/eat.wav",
		[AUDIO_NO_EAT]		= "res/audio/noAction.wav",
		[AUDIO_PICK]		= "res/audio/pick.wav",
		[AUDIO_NO_PICK]		= "res/audio/noAction.wav",
		[AUDIO_DROP]		= "res/audio/drop.wav",
		[AUDIO_NO_DROP]		= "res/audio/noAction.wav",
		[AUDIO_CONSOLE] 	= "res/audio/console.wav",
	};

	int i;
	for( i=AUDIO_NONE+1; i<AUDIO_NUM_ITEMS; i++) {
		log1( "Loading audio %s\n", fxPaths[ i]);
		Mix_Chunk *fx = Mix_LoadWAV( fxPaths[ i]);
		if( fx == NULL) {
			printf( "Failed to load audio '%s'. Error: %s\n", fxPaths[i], Mix_GetError());
			exit( 1);
		}
		audios.fx[ i] = fx;
	}

	int volumeStep = MIX_MAX_VOLUME / HEARING_DISTANCE;
	Mix_AllocateChannels( HEARING_DISTANCE);
	for( i=0; i<HEARING_DISTANCE; i++)
		Mix_Volume( i, (HEARING_DISTANCE - i) * volumeStep);
}

void audio_free() {
	int i;
	for( i=0; i<AUDIO_NUM_ITEMS; i++) {
		if( audios.fx[i] != NULL) {
			Mix_FreeChunk( audios.fx[ i]);
			audios.fx[ i] = NULL;
		}
	}
	Mix_CloseAudio();
}

