#ifndef BASIC_H
#define BASIC_H

#include <stdint.h>
#include <stdlib.h>

typedef uint8_t bool;
#define true 1
#define false 0

#define MIN( x, y) ((x) < (y) ? (x) : (y))
#define MAX( x, y) ((x) > (y) ? (x) : (y))

#define EXIT_IF( condition, ...) if( condition) { fprintf( stderr, __VA_ARGS__); exit(1); }

#endif
