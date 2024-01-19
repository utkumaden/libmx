#include "mx/io/stream.h"
#include "mx/assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static mx_stream_flags mx_stdin_flags(mx_stream_t str)
{
    return MX_STREAM_OPEN | (feof(stdin) ? MX_STREAM_EOF : 0);
}

static mx_len_t mx_stdin_read(mx_stream_t str, void *dst, mx_len_t size)
{
    return (mx_len_t)fread(dst, 1, (size_t)size, stdin);
}

MX_IMPL mx_stream_t mx_get_stdin(void)
{
    const static mx_stream_base_t str = {
        .flags = mx_stdin_flags,
        .read = mx_stdin_read,
        .write = NULL,
        .seek = NULL,
        .close = NULL
    };

    return &str;
}

static mx_stream_flags mx_stdxxx_flags(mx_stream_t str)
{
    return MX_STREAM_OPEN;
}

static mx_len_t mx_stdout_write(mx_stream_t str, const void *src, mx_len_t size)
{
    return (mx_len_t)fwrite(src, 1, (size_t)size, stdout);
}

static mx_len_t mx_stderr_write(mx_stream_t str, const void *src, mx_len_t size)
{
    return (mx_len_t)fwrite(src, 1, (size_t)size, stderr);
}

MX_IMPL mx_stream_t mx_get_stdout(void)
{
    const static mx_stream_base_t str = {
        .flags = mx_stdxxx_flags,
        .write = mx_stdout_write,
        .read = NULL,
        .seek = NULL,
        .close = NULL
    };

    return &str;
}

MX_IMPL mx_stream_t mx_get_stderr(void)
{
    const static mx_stream_base_t str = {
        .flags = mx_stdxxx_flags,
        .write = mx_stderr_write,
        .read = NULL,
        .seek = NULL,
        .close = NULL
    };

    return &str;
}

typedef struct mx_stdc_stream_t
{
    mx_stream_base_t base;
    FILE *file;
} mx_stdc_stream_t;

#define THIS_FILE(x) (((mx_stdc_stream_t*)(str))->file)

static mx_stream_flags mx_stdc_flags(mx_stream_t str)
{
    mx_stream_flags flags = 0;
    FILE *f = THIS_FILE(str);

    if (fileno(f) != -1)
        flags |= MX_STREAM_OPEN;
    if (feoff(f))
        flags |= MX_STREAM_EOF;

    return flags;
}

static mx_len_t mx_stdc_read(mx_stream_t str, void *dst, mx_len_t size)
{
    FILE *f = THIS_FILE(str);
    return (mx_len_t)fread(dst, 1, (size_t)size, f);
}

static mx_len_t mx_stdc_write(mx_stream_t str, const void *src, mx_len_t size)
{
    FILE *f = THIS_FILE(str);
    return (mx_len_t)fwrite(src, 1, (size_t)size, f);
}

static mx_len_t mx_stdc_seek(mx_stream_t str, mx_len_t offset, mx_seek_origin origin)
{
    FILE *f = THIS_FILE(str);
    int stdc_origin;

    switch (origin)
    {
    case MX_SEEK_START: stdc_origin = SEEK_SET; break;
    case MX_SEEK_CURRENT: stdc_origin = SEEK_CUR; break;
    case MX_SEEK_END: stdc_origin = SEEK_END; break;
    }

    fseek(f, offset, stdc_origin);
    return (mx_len_t)ftell(f);
}

static void mx_stdc_close(mx_stream_t str)
{
    FILE *f = THIS_FILE(str);

    fclose(f);
    free((void*)str);
}

const static mx_stream_base_t mx_stdc_stream_base =
{
    .flags = mx_stdc_flags,
    .read = mx_stdc_read,
    .write = mx_stdc_write,
    .seek = mx_stdc_seek,
    .close = mx_stdc_close
};

MX_API mx_stream_t mx_open(const char *file, mx_open_flags flags)
{
    MX_ASSERT_PTR(file, "File path cannot be null.");

    char mode[16] = "b"; 

    if (flags & MX_OPEN_READ)
    {
        strcat(mode, "r");
    }

    if (flags & MX_OPEN_WRITE)
    {
        strcat(mode, "w");
    }

    if (flags & MX_OPEN_APPEND)
    {
        strcat(mode, "a");
    }

    FILE *f = fopen(file, mode);

    if (f == NULL)
        return NULL;

    mx_stdc_stream_t *str = malloc(sizeof(*str));
    memcpy(&str->base, &mx_stdc_stream_base, sizeof mx_stdc_stream_base);

    if (!(flags & (MX_OPEN_APPEND|MX_OPEN_WRITE)))
    {
        str->base.write = NULL;
    }

    return str;
}

MX_IMPL mx_len_t mx_read(mx_stream_t str, void *dst, mx_len_t size)
{
    MX_ASSERT_SELFPTR(str);
    MX_ASSERT_PTR(dst, "Destination cannot be null");
    MX_ASSERT(size >= 0, "Must be non-negative.");
    MX_ASSERT(str->read, "Stream has no reader.");
    return str->read(str, dst, size);
}

MX_IMPL mx_len_t mx_write(mx_stream_t str, void *dst, mx_len_t size)
{
    MX_ASSERT_SELFPTR(str);
    MX_ASSERT_PTR(dst, "Source cannot be null");
    MX_ASSERT(size >= 0, "Must be non-negative.");
    MX_ASSERT(str->write, "Stream has no writer.");
    return str->write(str, dst, size);
}

MX_IMPL mx_len_t mx_seek(mx_stream_t str, mx_len_t offset, mx_seek_origin origin)
{
    MX_ASSERT_SELFPTR(str);
    MX_ASSERT(str->seek, "Stream has no seeker.");
    return str->seek(str, offset, origin);
}

MX_IMPL mx_stream_flags mx_sflags(mx_stream_t str)
{
    MX_ASSERT_SELFPTR(str);
    if (str->flags)
    {
        return str->flags(str);
    }
    else
    {
        return 0;
    }
}

MX_IMPL void mx_close(mx_stream_t str)
{
    if (!str) return;
    if (!str->close) return;
    str->close(str);
}