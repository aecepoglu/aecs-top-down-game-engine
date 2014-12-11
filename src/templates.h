#ifndef BRUSH_TEMPLATE_H
#define BRUSH_TEMPLATE_H

#include "object.h"
#include "basic.h"

#define MAX_TEMPLATES_COUNT 8
#define MAX_TEMPLATE_NAME_LENGTH 10

struct ObjectTemplate {
    struct object *obj;
    char *name;
    int aiType;
};
struct ObjectTemplate *objectTemplates[MAX_TEMPLATES_COUNT];
int templatesCount;

struct ObjectTemplate* template_create( int templateIndex, const char *name, int aiType, const struct object *o );
void template_remove( int index);
void templates_load();
void templates_save();
bool template_apply( struct object *to, int templateNo);

#endif
