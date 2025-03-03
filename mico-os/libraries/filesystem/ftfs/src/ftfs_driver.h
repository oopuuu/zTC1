/*
 * ftfs.h
 */
#ifndef _FTFS_H_
#define _FTFS_H_

#include <stddef.h>
#include <fs.h>
#include "mico.h"

#define FT_MAGIC        {0x88,0x88,0x88,0x88,0x63,0x6F,0x7A,0x79}
#define FT_VERSION      "0100"

#define FT_MAX_FILENAME 24

#define ftfs_e(...)				\
	wmlog_e("ftfs", ##__VA_ARGS__)
#define ftfs_w(...)				\
	wmlog_w("ftfs", ##__VA_ARGS__)

#ifdef CONFIG_FTFS_DEBUG
#define ftfs_d(...)				\
	wmlog("ftfs", ##__VA_ARGS__)
#else
#define ftfs_d(...)
#endif

/** FTFS Error Codes
 */

typedef enum {
	E_FTFS_ERRNO_BASE,
	/** Error while performing ftfs test init */
	E_FTFS_INIT_FAIL,
	/** Invalid file pointer */
	E_FTFS_INVALID_FILE,
	/** Ftfs seek failed */
	E_FTFS_SEEK_FAIL,
}ftfs_errno;

typedef struct {
	uint8_t magic[8];
	uint32_t crc;
	uint32_t obsolete;
	uint32_t backend_version;
	uint8_t version[4];
}FT_HEADER;

struct fs;

typedef struct {
	uint32_t offset;
	uint32_t length;
	uint32_t fp;
	struct ftfs_super *sb;
} FT_FILE;

/* The internal FTFS 'ft_entry' type
 * is exported here for quicker file size lookup and to enable 'ls' command 
 * -like functionality to be easily implemented.
 */
struct ft_entry {
	char name[FT_MAX_FILENAME];
	uint32_t offset;
	uint32_t length;
};

#define FTFS_MAX_FILE_DESCRIPTORS 16
struct ftfs_super {
	/* This should always be the first member of this structure. We use the
	 * same pointer at times as struct fs * or at times as struct
	 * ftfs_super *. Of course we could do the container_of() macro as in
	 * the Linux kernel, but am not sure of the impact of that across
	 * multiple compilers.  */
	struct fs fs;
	FT_FILE fds[FTFS_MAX_FILE_DESCRIPTORS];
	uint32_t fds_mask;
	uint32_t active_addr;
	unsigned fs_crc32;
};

static inline struct ftfs_super *f_to_ftfs_sb(file *f)
{
	FT_FILE *fp = (FT_FILE *)f;
	return fp->sb;
}

static inline struct ftfs_super *fs_to_ftfs_sb(struct fs *fs)
{
	return (struct ftfs_super *)fs;
}

/*
 * these functions are similar to fopen(), fclose(), fread(), ftell(), and
 * fseek() in stdio.h 
 **/

/** Initialize FTFS
 * 
 * This function initializes the File Table Filesystem module.
 * 
 *
 * \param[out] sb the ftfs superblock to be used for processing
 * \param[in] fd the flash descriptor
 * \return fs which is an opaque filesystem handle. This should be passed to
 * subsequent open calls. The points to a member within the 'sb' that is passed
 * as an argument.
 */
struct fs *ftfs_init(struct ftfs_super *sb, mico_partition_t partition);

/** Open a file
 *
 * This function is analogous to the fopen() call.
 * 
 * \param fs The opaque filesystem handle returned on filesystem initialization
 * \param path The path of the file to open
 * \param mode The mode to open the file in
 *
 * \return A pointer to a file handle to be used in subsequent calls
 * to fread etc. or NULL in case of error.
 */
file *ft_fopen(struct fs *fs, const char *path, const char *mode);

/** Close a file
 *
 * This function is analogous to the fclose() call.
 * 
 * \param f The file handle returned in the ft_fopen() call.
 *
 * \return E_FTFS_INVALID_FILE if invalid or NULL file is passed.
 * \return SUCCESS if operation successful.
 */
int ft_fclose(file *f);

/** Read from a file
 *
 * This function is analogous to the fread() call.
 * 
 * \param ptr A pointer to the buffer to read data in.
 * \param size The size of a single data element
 * \param nmemb The number of elements of data to read
 * \param f The file handle returned in the ft_fopen() call
 *
 * \return The length of the data read in bytes.
 */
size_t ft_fread(void *ptr, size_t size, size_t nmemb, file *f);

size_t ft_fwrite(const void *ptr, size_t size, size_t nmemb, file *f);

/** Get current file position
 *
 * This function is analogous to the ftell() call.
 * 
 * \param f The file handle returned in the ft_fopen() call.
 *
 * \return The current read offset in the file
 */
long ft_ftell(file *f);

/** Seek to an offset within a file
 *
 * This function is analogous to the fseek() call.
 * 
 * \param f The file handle returned in the ft_fopen() call.
 * \param offset The number of bytes with which to move the read offset.
 * \param whence Either of SEEK_SET, SEEK_CUR, or SEEK_END, to indicate from the
 * start of the file, from the current offset or from the end of the file
 * respectively.
 *
 * \return E_FTFS_SEEK_FAIL if seek operation fail.
 * \return SUCCESS if seek operation success.
 */
int ft_fseek(file *f, long offset, int whence);

/** Read FTFS Header
 *
 * \param fh Pointer to the FT_HEADER that should be filled
 * \param address The address where the FTFS starts
 * \param fl_dev The mdev_t pointer for the flash device
 *
 * \return FAIL if header read operation fails.
 * \return SUCCESS if header read operation is successful.
 */
static inline int ft_read_header(FT_HEADER *fh, uint32_t address)
{
  uint32_t offset = 0;
  return MicoFlashRead( MICO_PARTITION_FILESYS, &offset, (uint8_t *)fh, sizeof(FT_HEADER) );
}

int ft_is_valid_magic(uint8_t *magic);

/** Enable FTFS CLI
 *
 * The CLI can be enabled for only one of the FTFS's in the system.
 *
 * \param[in] fs the filesystem to use for ftfs commands
 * \note This function can be directly called by the application after ftfs
 * is initialized.
 *
 * \return E_FTFS_INIT_FAIL if cli registration failed.
 * \return SUCCESS if cli registration is successful.
 */
int ftfs_cli_init(struct fs *fs);

#endif /* _FTFS_H_ */
