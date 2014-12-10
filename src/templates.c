#include <string.h>
#include <stdio.h>
#include "templates.h"


#define TEMPLATES_PATH "templates.csv"
#define DELIMETER ","
#define MAX_NAME_LENGTH 10

#define CHECK_STRTOK_RESULT( string, no, name) EXIT_IF( string == NULL, "Error at line %d. Line ended but was expeting %s\n", no, name)

bool template_apply( struct object *to, int templateNo) {
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

struct ObjectTemplate* template_create( int templateIndex, const char *name, int aiType, const struct object *o ) {
    
	struct ObjectTemplate *template = (struct ObjectTemplate*)malloc(sizeof(struct ObjectTemplate));
	template->obj = (struct object*)malloc(sizeof(struct object));

	int nameLen = (int)strlen(name);
	EXIT_IF( nameLen > MAX_NAME_LENGTH, "template name %s is too long(%d) in line %d. Must be < %d\n", name, nameLen, templateIndex, MAX_NAME_LENGTH);
	template->name = (char*)calloc( nameLen+1, sizeof(char));
	memcpy( template->name, name, (strlen(name) + 1)*sizeof(char));

    template->aiType = aiType;
    template->obj->ai = NULL;

    template->obj->type = o->type;
    template->obj->health = o->health;
    template->obj->maxHealth = o->maxHealth;
    template->obj->healthGiven = o->healthGiven;
    template->obj->isPickable = o->isPickable;
    template->obj->isMovable = o->isMovable;
    template->obj->attack = o->attack;
    template->obj->defence = o->defence;

    return template;
}

void parseTemplateLine( char *line, int lineNo) {
	char *string;
	const char *delim = DELIMETER;

	struct ObjectTemplate *template = (struct ObjectTemplate*)malloc(sizeof(struct ObjectTemplate));
	template->obj = (struct object*)malloc(sizeof(struct object));

    struct object obj;
    char *templateName;
    int aiType;

	template->obj->id = 0;

	string = strtok( line, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "name");
    templateName = string;

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "obj-type");
	obj.type = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "ai-type");
	aiType = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "health");
	obj.health = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "maxHealth");
	obj.maxHealth = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "healthGiven");
	obj.healthGiven = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "isMovable");
	obj.isMovable = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "isPickable");
	obj.isPickable = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "attack");
	obj.attack = atoi( string);

	string = strtok( NULL, delim);
	CHECK_STRTOK_RESULT( string, lineNo, "defence");
	obj.defence = atoi( string);

    objectTemplates[ lineNo] = template_create( lineNo, templateName, aiType, &obj);
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
		printf("couldn't open template file. returning.\n");
		return;
    }

	int i;
	for( i=0; i<MAX_TEMPLATES_COUNT; i++) {
		struct ObjectTemplate *t = objectTemplates[i];
		if( t == NULL)
			break;

		fprintf( fp, "%s,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
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

void template_remove( int index) {
	if( index < 0 || index >= MAX_TEMPLATES_COUNT)
		return;
	
	struct ObjectTemplate *t = objectTemplates[index];
	if( t == NULL)
		return;
	
	free( t->obj);
	free( t->name);
	free( t);
}
