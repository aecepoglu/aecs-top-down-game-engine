#include <string.h>
#include <stdio.h>
#include "templates.h"


#define TEMPLATES_PATH "templates.csv"
#define DELIMETER ","
#define MAX_NAME_LENGTH 10

#define CHECK_STRTOK_RESULT( string, no, name) EXIT_IF( string == NULL, "Error at line %d. Line ended but was expeting %s\n", no, name)

bool applyObjTemplate( struct object *to, int templateNo) {
    struct ObjectTemplate *from = NULL;
    if( templateNo >= 0 && templateNo < MAX_TEMPLATES_COUNT)
        from = objectTemplates[ templateNo];

	if( from != NULL && to != NULL) {
		if( from->obj->id != 0)
			to->id = 0;
		to->type = from->obj->type;
		to->health = from->obj->health;
		to->maxHealth = from->obj->maxHealth;
		to->healthGiven = from->obj->healthGiven;
		to->isMovable = from->obj->isMovable;
		to->isPickable = from->obj->isPickable;
		to->attack = from->obj->attack;
		to->defence = from->obj->defence;
		//TODO copy AI

		return true;
	}
	else
		return false;
}

void parseTemplateLine( char *line, int lineNo) {
	char *string;
	const char *delim = DELIMETER;

	struct ObjectTemplate *template = (struct ObjectTemplate*)malloc(sizeof(struct ObjectTemplate));
	template->obj = (struct object*)malloc(sizeof(struct object));

	template->obj->id = 0;

	string = strtok( line, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "name");
	int nameLen = (int)strlen(string);
	EXIT_IF( nameLen > MAX_NAME_LENGTH, "template name %s is too long(%d) in line %d. Must be < %d\n", string, nameLen, lineNo, MAX_NAME_LENGTH);
	template->name = (char*)calloc( nameLen+1, sizeof(char));
	memcpy( template->name, string, (strlen(string) + 1)*sizeof(char));

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "obj-type");
	template->obj->type = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "ai-type");
	template->aiType = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "health");
	template->obj->health = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "maxHealth");
	template->obj->maxHealth = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "healthGiven");
	template->obj->healthGiven = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "isMovable");
	template->obj->isMovable = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "isPickable");
	template->obj->isPickable = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "attack");
	template->obj->attack = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "defence");
	template->obj->defence = atoi( string);

    objectTemplates[ lineNo] = template;
}


void templates_load() {
	int i;
	for( i=0; i<MAX_TEMPLATES_COUNT; i++) {
		objectTemplates[i] = NULL;
	}

	FILE *fp = fopen( TEMPLATES_PATH, "r");
	if( fp == NULL) {
		return;
    }

	int lineNo = 0;
	char line[BUFSIZ];

	while( fgets( line, BUFSIZ, fp) ) {
		if( lineNo > MAX_TEMPLATES_COUNT) {
			fprintf( stderr, "Max %d templates supported.\n", MAX_TEMPLATES_COUNT);
			break;
		}

		lineNo ++;

		parseTemplateLine( line, lineNo);
	}

	fclose( fp);
}

void templates_save() {
	FILE *fp = fopen( TEMPLATES_PATH, "w");
	if( fp == NULL) {
		return;
    }

	int i;
	for( i=0; i<MAX_TEMPLATES_COUNT; i++) {
		struct ObjectTemplate *t = objectTemplates[i];
		if( t == NULL)
			break;

		fprintf( fp, "%s,%d,%d,%d,%d,%d,%d,%d,%d,%d",
			t->name,
			t->obj->type,
			t->aiType,
			t->obj->health,
			t->obj->maxHealth,
			t->obj->healthGiven,
			t->obj->isMovable,
			t->obj->isPickable,
			t->obj->attack,
			t->obj->defence
		);
	}

	fclose( fp);
}

