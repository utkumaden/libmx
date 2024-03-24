#ifndef _MX_IO_STREAM_H_
#define _MX_IO_STREAM_H_

#include "mx/base.h"
#include "mx/trait.h"

typedef enum IStream_SeekOrigin
{
    ISTREAM_SEEK_START,
    ISTERAM_SEEK_CURRENT,
    ISTREAM_SEEK_END,
} IStream_SeekOrigin;

typedef enum mx_stream_flags
{
    MX_STREAM_OPEN = 1 << 0,
    MX_STREAM_EOF  = 1 << 1,
} mx_stream_flags;

typedef struct IStream
{
    IObject Object;

    mx_stream_flags (*get_flags)(void *self);
    mx_len_t (*read)(void *self, char *buffer, mx_len_t max);
    mx_len_t (*seek)(void *self, mx_len_t offset, IStream_SeekOrigin origin);
    mx_len_t (*write)(void* self, const void *src, mx_len_t size);
    void (*close)(void *self);
} IStream;
fatptr_define(IStream);

MX_INLINE fatptr_t(IObject) IStream_AsIObject(fatptr_t(IStream) str)
{
    return fat_new(str.ptr, str.traits->Object, IObject);
}

MX_INLINE mx_stream_flags IStream_flags(fatptr_t(IStream) str)
{
    return fatptr_vcall(str, get_flags);
}

MX_INLINE mx_len_t IStream_read(fatptr_t(IStream) str, char *buffer, mx_len_t max)
{
    return fatptr_vcall(str, read, buffer, max);
}

MX_INLINE mx_len_t IStream_seek(fatptr_t(IStream) str, mx_len_t offset, IStream_SeekOrigin origin)
{
    return fatptr_vcall(str, seek, offset, origin);
}

MX_INLINE mx_len_t IStream_write(fatptr_t(IStream) str, const void *buffer, mx_len_t size)
{
    return fatptr_vcall(str, write, buffer, size);
}

MX_INLINE void IStream_close(fatptr_t(IStream) str)
{
    fatptr_vcall(str, close);
}

typedef enum mx_open_flags
{
    MX_OPEN_READ   = 1 << 0,
    MX_OPEN_WRITE  = 1 << 1,
    MX_OPEN_APPEND = 1 << 2,
    MX_OPEN_NEW    = 1 << 3,
} mx_open_flags;

MX_API fatptr_t(IStream) mx_get_stdin(void);
MX_API fatptr_t(IStream) mx_get_stdout(void);
MX_API fatptr_t(IStream) mx_get_stderr(void);
MX_API fatptr_t(IStream) mx_open(const char *file, mx_open_flags flags);

#define mx_stdin  (mx_get_stdin())
#define mx_stdout (mx_get_stdout())
#define mx_stderr (mx_get_stderr())

#endif