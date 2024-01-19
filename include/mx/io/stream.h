#ifndef _MX_IO_STREAM_H_
#define _MX_IO_STREAM_H_

#include "mx/base.h"

typedef enum mx_open_flags
{
    MX_OPEN_READ   = 1 << 0,
    MX_OPEN_WRITE  = 1 << 1,
    MX_OPEN_APPEND = 1 << 2,
    MX_OPEN_NEW    = 1 << 3,
} mx_open_flags;

typedef enum mx_seek_origin
{
    MX_SEEK_START,
    MX_SEEK_CURRENT,
    MX_SEEK_END,
} mx_seek_origin;

typedef enum mx_stream_flags
{
    MX_STREAM_OPEN = 1 << 0,
    MX_STREAM_EOF  = 1 << 1,
} mx_stream_flags;

typedef struct mx_stream_base_t
{
    mx_stream_flags (*flags)(mx_stream_t str);
    mx_len_t (*read)(mx_stream_t str, void *dst, mx_len_t size);
    mx_len_t (*write)(mx_stream_t str, const void *src, mx_len_t size);
    mx_len_t (*seek)(mx_stream_t str, mx_len_t offset, mx_seek_origin origin);
    void     (*close)(mx_stream_t str);

} mx_stream_base_t;
typedef const mx_stream_base_t* mx_stream_t;

MX_API mx_stream_t mx_get_stdin(void);
MX_API mx_stream_t mx_get_stdout(void);
MX_API mx_stream_t mx_get_stderr(void);
MX_API mx_stream_t mx_open(const char *file, mx_open_flags flags);
MX_API mx_stream_flags mx_sflags(mx_stream_t str);
MX_API mx_len_t mx_read(mx_stream_t str, void *dst, mx_len_t size);
MX_API mx_len_t mx_write(mx_stream_t str, const void *dst, mx_len_t size);
MX_API mx_len_t mx_seek(mx_stream_t str, mx_len_t offset, mx_seek_origin origin);
MX_API void mx_close(mx_stream_t str);

#define mx_stdin  (mx_get_stdin())
#define mx_stdout (mx_get_stdout())
#define mx_stderr (mx_get_stderr())


#endif