#include <SDL.h>
#include "object.h"
#include "log.h"

typedef bool (brushFun)(unsigned int, unsigned int, int);

struct brushState {
	char *name;
	int len;
	brushFun* brush;
	int variant;
	struct stateTransition *stateTransitions;
};
struct stateTransition {
	SDL_Keycode key;
	struct brushState target;
};

brushFun defaultBrush;
brushFun drawObject;
brushFun drawTerrain;
brushFun drawAI;

#define no_var 0 
#define no_trans 0
struct brushState initialBrushState = 
{"null", 1, defaultBrush, no_var, (struct stateTransition[]) {
	//0
	{SDLK_1, { "create", 3, defaultBrush, no_var, (struct stateTransition[]) {
		//0
		{SDLK_1, { "object", 0, drawObject, go_monster, no_trans} },
		//1
		{SDLK_2, { "terrain", 2, defaultBrush, no_var, (struct stateTransition[]) {
			//0
			{SDLK_1, { "ground", 0, drawTerrain, terrain_none, no_trans} },
			//1
			{SDLK_2, { "wall", 0, drawTerrain, terrain_wall, no_trans} }
		}}},
		//2
		{SDLK_3, { "AI", 1, defaultBrush, no_var, (struct stateTransition[]) {
			{SDLK_1, { "left-turner", 0, drawAI, ai_leftTurner, no_trans} },
		}}},
	}}}
}};
#undef no_var
#undef no_trans

//void (*updateBrushState( struct brushState *(*curState), SDL_Keycode key))( unsigned int, unsigned int, int) {
brushFun* updateBrushState( struct brushState **curState, int *brushVariant, SDL_Keycode key) {
	int i;
	for( i=0; i< (*curState)->len; i++)
		if( (*curState)->stateTransitions[i].key == key) {
			//go to the next state
			(*curState) = &((*curState)->stateTransitions[i].target);
			log0("state changed to %s\n", (*curState)->name);
			//if the new state is a final state
			if( (*curState)->stateTransitions == 0) {
				brushFun* result = (*curState)->brush;
				*brushVariant = (*curState)->variant;
				log0("reseting brush state\n");
				//reset the state
				(*curState) = &initialBrushState;

				return result;
			}
			else
				return defaultBrush;
		}
	return defaultBrush;
}

