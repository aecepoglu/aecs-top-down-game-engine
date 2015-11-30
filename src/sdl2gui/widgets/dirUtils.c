#include "dirUtils.h"

#include <dirent.h>
#include <string.h>
#include <stdlib.h>

int compareMyFile(const void *a, const void *b) {
	return strcmp( ((struct MyFile*)a)->name, ((struct MyFile*)b)->name );
}

struct MyDirectory *crossplatformDir(const char *path, const char *filename) {
	DIR *dp;
	struct dirent *ep;     

	dp = opendir (path);
	
	if (dp != NULL) {
		size_t count = 0;
		struct MyDirectory *result = (struct MyDirectory*)malloc(sizeof(struct MyDirectory));

		ep = readdir(dp);

		while (ep) {
			if(filename == 0 || ep->d_type == DT_DIR || strcmp(ep->d_name, filename) == 0)
				count ++;

			ep = readdir(dp);
		}

		rewinddir(dp);
		ep = readdir(dp);

		result->files = (struct MyFile*)calloc(count, sizeof(struct MyFile));
		result->count = 0;
		while (ep && result->count < count) {
			if(filename == 0 || ep->d_type == DT_DIR || strcmp(ep->d_name, filename) == 0) {
				result->files[result->count].name = strdup(ep->d_name);
				result->files[result->count].isDir = ep->d_type == DT_DIR;
				result->count ++;
			}
			
			ep = readdir(dp);
		}

		closedir(dp);

		qsort(result->files, result->count, sizeof(struct MyFile), &compareMyFile);

		return result;
	}
	else
		return 0;
}

void destroyDirectory(struct MyDirectory *d) {
	free(d->files);
	free(d);
}
