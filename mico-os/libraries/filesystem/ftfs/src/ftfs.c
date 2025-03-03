/*
 * ftfs.c
 */
#include <string.h>
#include "ftfs_driver.h"
#include "mico_platform.h"
#define ftfs_log(format, ...)  custom_log("ftfs", format, ##__VA_ARGS__)

extern const mico_logic_partition_t mico_partitions[];

static char magic[8] = FT_MAGIC;

int ft_fclose( file *f )
{
    unsigned int i;
    struct ftfs_super *sb = f_to_ftfs_sb( f );

    if ( !f )
        return E_FTFS_INVALID_FILE;

    for ( i = 0; i < sizeof(sb->fds) / sizeof(sb->fds[0]); i++ )
    {
        if ( &sb->fds[i] == (FT_FILE *) f )
        {
            sb->fds_mask &= ~(1 << i);
            return kNoErr;
        }
    }

    return E_FTFS_INVALID_FILE;
}

static FT_FILE *_ft_fopen( struct fs *fs, const char *path, const char *mode )
{
    unsigned int i;
    FT_FILE *f = NULL;
    const char *name;
    struct ftfs_super *sb = (struct ftfs_super *) fs;

    struct ft_entry entry;
    uint32_t addr = sizeof(FT_HEADER);

    if ( path[0] == '/' )
        name = &path[1];
    else
        name = &path[0];

    do
    {
        MicoFlashRead( MICO_PARTITION_FILESYS, &addr, (uint8_t *) &entry, sizeof(entry) );

        if ( entry.name[0] == '\0' ) /* reached end of table */
        return NULL; /* file not found */
    } while ( strncmp( entry.name, name, FT_MAX_FILENAME ) );

    for ( i = 0; i < sizeof(sb->fds) / sizeof(sb->fds[0]);
        i++ )
    {
        if ( !(sb->fds_mask & 1 << i) )
        {
            f = &sb->fds[i];
            f->offset = entry.offset;
            f->length = entry.length;
            f->fp = 0;
            f->sb = sb;
            sb->fds_mask |= 1 << i;
            break;
        }
    }
    if ( i == sizeof(sb->fds) / sizeof(sb->fds[0]) )
        return NULL;
    return f;
}

file *ft_fopen( struct fs *fs, const char *path, const char *mode )
{
    if ( !fs || !path )
        return NULL;

    return (file *) _ft_fopen( fs, path, mode );
}

size_t ft_fread( void *ptr, size_t size, size_t nmemb, file *f )
{
    FT_FILE *stream = (FT_FILE *) f;
    int i;
    size_t len = 0;
    uint32_t addr = sizeof(FT_HEADER) + stream->offset + stream->fp;
    char *b = (char *) ptr;

    if ( !stream )
        return (size_t) -1;

    if ( stream->fp >= stream->length )
        return 0;

    for ( i = 0; i < nmemb; i++ )
    {
        if ( stream->fp + size >= stream->length )
            size = stream->length - stream->fp;
        MicoFlashRead( MICO_PARTITION_FILESYS, &addr, (uint8_t *) (b + len), size );
        len += size;
        stream->fp += size;
        if ( stream->fp >= stream->length )
            break;
    };

    return len;
}

size_t ft_fwrite( const void *ptr, size_t size, size_t nmemb, file *f )
{
    return (size_t) -1;
}

long ft_ftell( file *f )
{
    FT_FILE *stream = (FT_FILE *) f;
    if ( !stream )
        return 0;

    return (long) stream->fp;
}

int ft_fseek( file *f, long offset, int whence )
{
    FT_FILE *stream = (FT_FILE *) f;

    if ( !stream )
        return E_FTFS_SEEK_FAIL;

    switch ( whence )
    {
        case SEEK_SET:
            if ( offset < 0 || offset >= stream->length )
                return E_FTFS_SEEK_FAIL;
            stream->fp = offset;
            break;

        case SEEK_CUR:
            if ( offset + (long) stream->fp < 0 ||
                 offset + stream->fp >= stream->length )
                return E_FTFS_SEEK_FAIL;
            stream->fp += offset;
            break;

        case SEEK_END:
            if ( offset > 0 || offset + (long) stream->length < 0 )
                return E_FTFS_SEEK_FAIL;
            stream->fp = offset + stream->length;
            break;

        default:
            return E_FTFS_SEEK_FAIL;
    }

    return kNoErr;
}

int ft_is_valid_magic( uint8_t *test_magic )
{
    if ( memcmp( test_magic, magic, sizeof(magic) ) == 0 )
        return 1;
    return 0;
}

//int ftfs_load_entire_file_to_buffer(struct fs *fs, const char *file_name,
//				    char *buf, int max_len)
//{
//	ftfs_d("er: %s", file_name);
//
//	if (!fs || !file_name || !buf || !max_len)
//		return -WM_E_INVAL;
//
//	int rv;
//	FT_FILE *f = _ft_fopen(fs, file_name, "r");
//	if (!f) {
//		ftfs_log("(er) %s not found", file_name);
//		return -WM_E_NOENT;
//	}
//
//	if (f->length > max_len) {
//		ft_fclose((file *) f);
//		return -WM_E_NOSPC;
//	}
//
//	rv = ft_fread(buf, f->length, 1, (file *) f);
//	if (rv < 0) {
//		ftfs_log("(er) Could not read file: %d", rv);
//	}
//
//	ft_fclose((file *) f);
//
//	return rv;
//}

//int ftfs_get_filesize(struct fs *fs, const char *file_name)
//{
//	if (!fs || !file_name)
//		return -WM_E_INVAL;
//
//	int rv;
//	FT_FILE *f = _ft_fopen(fs, file_name, "r");
//	if (!f) {
//		ftfs_log("(fs) %s not found", file_name);
//		return -WM_E_NOENT;
//	}
//
//	rv = f->length;
//	ft_fclose((file *) f);
//	return rv;
//}

struct fs *ftfs_init( struct ftfs_super *sb, mico_partition_t partition )
{
    FT_HEADER sec;
    uint32_t start_addr = 0;
//    struct ftfs_super sb;
    mico_logic_partition_t *ftfs_partition;

    ftfs_partition = MicoFlashGetInfo( partition );

    start_addr = ftfs_partition->partition_start_addr;

    if ( ft_read_header( &sec, start_addr ) != kNoErr )
        return NULL;

    if ( !ft_is_valid_magic( sec.magic ) )
        return NULL;

    memset( sb, 0, sizeof(*sb) );
    sb->fs.fopen = ft_fopen;
    sb->fs.fclose = ft_fclose;
    sb->fs.fread = ft_fread;
    sb->fs.fwrite = ft_fwrite;
    sb->fs.ftell = ft_ftell;
    sb->fs.fseek = ft_fseek;

    memset( sb->fds, 0, sizeof(sb->fds) );
    sb->fds_mask = 0;

    sb->active_addr = start_addr;

    if ( sb->active_addr < 0 )
        return 0;

    /* Check CRC on each init? */
    sb->fs_crc32 = sec.crc;

    return (struct fs *) sb;
}

bool ft_is_content_valid( mico_partition_t partition, int be_ver )
{
    FT_HEADER sec1;
    bool ret = false;

    if ( ft_read_header( &sec1, mico_partitions[partition].partition_start_addr ) != kNoErr )
        goto ret;

    ftfs_log("part: be_ver: %ld", sec1.backend_version);

    if ( ft_is_valid_magic( sec1.magic ) && sec1.backend_version == be_ver )
    {
        ret = true;
    }

    ret:

    return ret;
}
