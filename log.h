#include <stdio.h>

#ifndef LOG_H
#define LOG_H

#define VERBOSE_3

#ifdef VERBOSE_0
	#define log0(...) fprintf(stdout, __VA_ARGS__)
	#define log1(...)
	#define log2(...)
	#define log3(...)
#else
	#ifdef VERBOSE_1
		#define log0(...) fprintf(stdout, __VA_ARGS__)
		#define log1(...) fprintf(stdout, __VA_ARGS__)
		#define log2(...)
		#define log3(...)
	#else
		#ifdef VERBOSE_2
			#define log0(...) fprintf(stdout, __VA_ARGS__)
			#define log1(...) fprintf(stdout, __VA_ARGS__)
			#define log2(...) fprintf(stdout, __VA_ARGS__)
			#define log3(...)
		#else
			#ifdef VERBOSE_3
				#define log0(...) fprintf(stdout, __VA_ARGS__)
				#define log1(...) fprintf(stdout, __VA_ARGS__)
				#define log2(...) fprintf(stdout, __VA_ARGS__)
				#define log3(...) fprintf(stdout, __VA_ARGS__)
			#else
				#define log0(...)
				#define log1(...)
				#define log2(...)
				#define log3(...)
			#endif
		#endif
	#endif
#endif

#endif //LOG_H
