#include "audio.h"
#include "log.h"
#include <stdlib.h>

void audio_init() {
	if( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 2048) < 0) {
		printf( "SDL_mixer couldn't initialize. Error: %s\n", Mix_GetError());
	}

	char *fxPaths[AUDIO_NUM_ITEMS] = {
		[AUDIO_MOVE] 		= "res/audio/move.wav",
		[AUDIO_NO_MOVE] 	= "res/audio/noMove.wav",
		[AUDIO_INTERACT] 	= "res/audio/interact.wav",
		[AUDIO_NO_INTERACT] = "res/audio/noInteract.wav",
		[AUDIO_PUSH] 		= "res/audio/push.wav",
		[AUDIO_NO_PUSH] 	= "res/audio/noPush.wav",
		[AUDIO_HIT]			= "res/audio/hit.wav",
		[AUDIO_NO_HIT]		= "res/audio/noHit.wav",
		[AUDIO_EAT]			= "res/audio/eat.wav",
		[AUDIO_NO_EAT]		= "res/audio/noEat.wav",
		[AUDIO_PICK]		= "res/audio/pick.wav",
		[AUDIO_NO_PICK]		= "res/audio/noPick.wav",
		[AUDIO_DROP]		= "res/audio/drop.wav",
		[AUDIO_NO_DROP]		= "res/audio/noDrop.wav",
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

