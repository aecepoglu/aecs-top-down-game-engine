#ifndef BASIC_H
#define BASIC_H

#include <stdint.h>

typedef uint8_t bool;
#define true 1
#define false 0

#define MIN( x, y) ((x) < (y) ? (x) : (y))
#define MAX( x, y) ((x) > (y) ? (x) : (y))

#endif
