#ifndef __FS__H__
#define __FS__H__

#include <stddef.h>

/* the file type is opaque to the caller. */
typedef int *file;

#define FS_EOF -1
#define FS_EINVAL -2
#define FS_EUNIMPLEMENTED -3
#define FS_ENOENT -4
#define FS_EMFILE -5
#define FS_EROFS -6
#define FS_EOVERFLOW -6

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

struct fs {
	int fs_errno;
	file *(* fopen)(struct fs *fs, const char *path, const char *mode);
	int (* fclose)(file *f);
	size_t (* fread)(void *ptr, size_t size, size_t nmemb, file *f);
	size_t (* fwrite)(const void *ptr, size_t size, size_t nmemb, file *f);
	long (* ftell)(file *f);
	int (* fseek)(file *f, long offset, int whence);
};

#endif /* __FS__H__ */
