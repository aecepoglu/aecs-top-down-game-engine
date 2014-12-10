#ifndef BRUSH_TEMPLATE_H
#define BRUSH_TEMPLATE_H

#include "object.h"
#include "basic.h"

#define MAX_TEMPLATES_COUNT 8

struct ObjectTemplate {
    struct object *obj;
    char *name;
    int aiType;
};
struct ObjectTemplate *objectTemplates[MAX_TEMPLATES_COUNT];
int templatesCount;

void templates_load();
void templates_save();

#endif
