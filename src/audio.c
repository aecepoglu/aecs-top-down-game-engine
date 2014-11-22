#include "audio.h"
#include "log.h"

void audio_init() {
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 1, 2048) < 0) {
		printf( "SDL_mixer couldn't initialize. Error: %s\n", Mix_GetError());
	}

	char *fxPaths[AUDIO_NUM_ITEMS] = {
		[AUDIO_MOVE] = "res/audio/low.wav",
		[AUDIO_INTERACT] = "res/audio/high.wav",
		[AUDIO_CONSOLE] = "res/audio/medium.wav",
	};

	int i;
	for( i=0; i<AUDIO_NUM_ITEMS; i++) {
		log1( "Loading audio %s\n", fxPaths[ i]);
		Mix_Chunk *fx = Mix_LoadWAV( fxPaths[ i]);
		if( fx == NULL) {
			printf( "Failed to load test audio fx. Error: %s\n", Mix_GetError());
			exit( 1);
		}
		audios.fx[ i] = fx;
	}
}

void audio_free() {
	int i;
	for( i=0; i<AUDIO_NUM_ITEMS; i++) {
		Mix_FreeChunk( audios.fx[ i]);
		audios.fx[ i] = NULL;
	}
	Mix_CloseAudio();
}

