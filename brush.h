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
brushFun drawPlayer;
brushFun drawAI;
brushFun setDirection;
brushFun eraseObject;
brushFun eraseAI;

#define no_var 0
#define no_trans 0
struct brushState initialBrushState =
{"null", 2, defaultBrush, no_var, (struct stateTransition[]) {
	//0
	{SDLK_1, { "create/set", 4, defaultBrush, no_var, (struct stateTransition[]) {
		//0
		{SDLK_1, { "object", 6, defaultBrush, no_var, (struct stateTransition[]) {
			{SDLK_1, { "player", 0, drawPlayer, no_var, no_trans} },
			{SDLK_2, { "leftTurner", 0, drawObject, go_leftTurner, no_trans} },
			{SDLK_3, { "apple", 0, drawObject, go_apple, no_trans} },
			{SDLK_4, { "flower", 0, drawObject, go_flower, no_trans} },
			{SDLK_5, { "creeper", 0, drawObject, go_creeperPlant, no_trans} },
			{SDLK_6, { "peekaboo monster", 0, drawObject, go_peekaboo, no_trans} },
		}}},
		//1
		{SDLK_2, { "terrain", 2, defaultBrush, no_var, (struct stateTransition[]) {
			//0
			{SDLK_1, { "ground", 0, drawTerrain, terrain_none, no_trans} },
			//1
			{SDLK_2, { "wall", 0, drawTerrain, terrain_wall, no_trans} }
		}}},
		//2
		{SDLK_3, { "AI", 5, defaultBrush, no_var, (struct stateTransition[]) {
			{SDLK_1, { "left turner", 0, drawAI, ai_leftTurner, no_trans} },
			{SDLK_2, { "hungry left turner", 0, drawAI, ai_hungryLeftTurner, no_trans} },
			{SDLK_3, { "simple flower", 0, drawAI, ai_simpleFlower, no_trans} },
			{SDLK_4, { "creeper plant", 0, drawAI, ai_creeperPlant, no_trans} },
			{SDLK_5, { "peek-a-boo chaser", 0, drawAI, ai_peekaboo, no_trans} },
		}}},
		//3
		{SDLK_4, { "direction", 4, defaultBrush, no_var, (struct stateTransition[]) {
			{SDLK_1, { "up", 0, setDirection, dir_up, no_trans} },
			{SDLK_2, { "right", 0, setDirection, dir_right, no_trans} },
			{SDLK_3, { "down", 0, setDirection, dir_down, no_trans} },
			{SDLK_4, { "left", 0, setDirection, dir_left, no_trans} },
		}}}
	}}},
	{SDLK_2, { "erase", 2, defaultBrush, no_var, (struct stateTransition[]) {
		//0
		{SDLK_1, { "object", 6, eraseObject, no_var, no_trans }},
		//2
		{SDLK_2, { "AI", 5, eraseAI, no_var, no_trans }},
	}}}
}};
#undef no_var
#undef no_trans

//void (*updateBrushState( struct brushState *(*curState), SDL_Keycode key))( unsigned int, unsigned int, int) {
brushFun* updateBrushState( struct brushState **curState, int *brushVariant, SDL_Keycode key) {
	brushFun *result = defaultBrush;
	int i;
	for( i=0; i< (*curState)->len; i++)
		if( (*curState)->stateTransitions[i].key == key) {
			//go to the next state
			(*curState) = &((*curState)->stateTransitions[i].target);
			log0("state changed to %s\n", (*curState)->name);
			//if the new state is a final state
			if( (*curState)->stateTransitions == 0) {
				result = (*curState)->brush;
				*brushVariant = (*curState)->variant;
				log0("reseting brush state\n");
				//reset the state
				(*curState) = &initialBrushState;
			}
			break;
		}
	
	for( i=0; i< (*curState)->len; i++)
		log0("%d. %s\n", i+1, (*curState)->stateTransitions[i].target.name);
	log0("\n");

	return result;
}

