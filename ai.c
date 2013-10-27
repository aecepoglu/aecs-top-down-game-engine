#include "object.h" //has ai definitions
#include "ai.h"

/* Creates and returns an AI of given type.
	Returns null if there is no such AI 
*/
struct AI* createAI( enum aiType type) {
	int i;
	for(i=0; i<AICOUNT; i++) 
		if( aiTable[i].type == type)
			return aiTable[i].constructor();
	return 0;
}

