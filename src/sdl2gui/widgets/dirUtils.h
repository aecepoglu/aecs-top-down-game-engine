#ifndef DIR_UTILS_H
#define DIR_UTILS_H

struct MyFile {
	char *name;
	int isDir;
};

struct MyDirectory {
	struct MyFile *files;
	int count;
};

struct MyDirectory *crossplatformDir(const char *path, const char *filename);

void destroyDirectory(struct MyDirectory *d);

#endif
