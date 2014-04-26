#include <stdio.h>

#ifndef LOG_H
#define LOG_H

#define VERBOSE_1

#ifdef VERBOSE_0
	#define log0(...) fprintf(stdout, __VA_ARGS__)
	#define log1(...)
	#define log2(...)
#else
	#ifdef VERBOSE_1
		#define log0(...) fprintf(stdout, __VA_ARGS__)
		#define log1(...) fprintf(stdout, __VA_ARGS__)
		#define log2(...)
	#else
		#ifdef VERBOSE_2
			#define log0(...) fprintf(stdout, __VA_ARGS__)
			#define log1(...) fprintf(stdout, __VA_ARGS__)
			#define log2(...) fprintf(stdout, __VA_ARGS__)
		#else
			#define log0(...)
			#define log1(...)
			#define log2(...)
		#endif
	#endif
#endif

#endif //LOG_H
