#ifndef LOG_H
#define LOG_H

#define VERBOSE_0 //default verbose level

#ifdef VERBOSE_0
	#define log0(...) fprintf(stdout, __VA_ARGS__)
	#define log1(...)
#else
	#ifdef VERBOSE_1
		#define log0(...) fprintf(stdout, __VA_ARGS__)
		#define log1(...) fprintf(stdout, __VA_ARGS__)
	#else
		#define log0(...)
		#define log1(...)
	#endif
#endif

#endif //LOG_H
