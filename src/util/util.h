#ifndef UTIL_H
#define UTIL_H

char *my_strcut( const char *from, const char *to);
char* getDirPath( const char *file);
char *combineFilePaths( const char *dir, const char *file);

#define MAX_FILEPATH_LENGTH 255

#endif /*UTIL_H*/
