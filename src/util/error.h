#ifndef ERROR_H
#define ERROR_H

char errorMessage[256];

#define ERROR(...) do {fprintf(stderr, __VA_ARGS__); sprintf(errorMessage, __VA_ARGS__); } while(0)

#endif /*ERROR_H*/
