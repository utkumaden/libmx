#include "mx/io/stream.h"
#include "mx/assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct mx_file_t {
    FILE *f;
} mx_file_t;

static size_t mx_file_IObject_get_size(mx_file_t *self);
static size_t mx_file_IObject_to_string(mx_file_t *self, char *buffer, size_t max);
static void   mx_file_IObject_destruct(mx_file_t *self);

static mx_stream_flags mx_file_IStream_get_flags(mx_file_t *self);
static mx_len_t mx_file_IStream_read(mx_file_t *self, char *buffer, mx_len_t max);
static mx_len_t mx_file_IStream_seek(mx_file_t *self, mx_len_t offset, IStream_SeekOrigin origin);
static mx_len_t mx_file_IStream_write(mx_file_t *self, const char *buffer, mx_len_t max);
static void mx_file_IStream_close(mx_file_t *self);

const IObject fat_vtable(mx_file_t, IObject) = {
    .get_size  = mx_file_IObject_get_size,
    .get_type  = NULL,
    .to_string = mx_file_IObject_to_string,
    .destruct  = mx_file_IObject_destruct
};

const IStream fat_vtable(mx_file_t, IStream) = {
    .Object = {
        .get_size  = mx_file_IObject_get_size,
        .get_type  = NULL,
        .to_string = mx_file_IObject_to_string,
        .destruct  = mx_file_IObject_destruct
    },
    .get_flags = mx_file_IStream_get_flags,
    .read = mx_file_IStream_read,
    .seek = mx_file_IStream_seek,
    .write = mx_file_IStream_write,
    .close = mx_file_IStream_close,
};

static size_t mx_file_IObject_get_size(mx_file_t *self)
{
    return sizeof(self);
}

static size_t mx_file_IObject_to_string(mx_file_t *self, char *buffer, size_t max)
{
    const char in[] = "stdin";
    const char out[] = "stdout";
    const char err[] = "stderr";

    if (self->f == stdin)
    {
        strncpy(buffer, in, max);
        return max > sizeof(in) ? sizeof(in) : max;
    }
    else if (self->f == stdout)
    {
        strncpy(buffer, out, max);
        return max > sizeof(out) ? sizeof(out) : max;
    }
    else if (self->f == stderr)
    {
        strncpy(buffer, err, max);
        return max > sizeof(err) ? sizeof(err) : max;
    }
    else
    {
        return snprintf(buffer, max, "FILE* %p", self->f);
    }
}

static void mx_file_IObject_destruct(mx_file_t *self)
{
    fatptr_vcall(fat_new(self, fat_vtable(mx_file_t, IStream), IStream), close);
    free(self);
}

static mx_stream_flags mx_file_IStream_get_flags(mx_file_t *self)
{
    if (self->f == NULL)
        return MX_STREAM_EOF;
    
    mx_stream_flags flags = MX_STREAM_OPEN;

    flags |= !!feof(self->f) * MX_STREAM_EOF;
    
    return flags;
}

static mx_len_t mx_file_IStream_read(mx_file_t *self, char *buffer, mx_len_t max)
{
    return fread(buffer, 1, max, self->f);
}

static mx_len_t mx_file_IStream_seek(mx_file_t *self, mx_len_t offset, IStream_SeekOrigin origin)
{
    return fseek(self->f, offset, origin);
}

static mx_len_t mx_file_IStream_write(mx_file_t *self, const char *buffer, mx_len_t max)
{
    return fwrite(buffer, 1, max, self->f);
}

static void mx_file_IStream_close(mx_file_t *self)
{
    if (self->f == NULL)
        return;
    else if (self->f != stdin || self->f != stdout || self->f != stderr)
        fclose(self->f);

    self->f = NULL;
}

MX_API fatptr_t(IStream) mx_get_stdin(void)
{
    static bool init = false;
    static mx_file_t fd;

    if (!init)
    {
        fd.f = stdin;
        init = true;
    }

    return fat_cast(fd, mx_file_t, IStream);
}

MX_API fatptr_t(IStream) mx_get_stdout(void)
{
    static bool init = false;
    static mx_file_t fd;

    if (!init)
    {
        fd.f = stdout;
        init = true;
    }

    return fat_cast(fd, mx_file_t, IStream);
}

MX_API fatptr_t(IStream) mx_get_stderr(void)
{
    static bool init = false;
    static mx_file_t fd;

    if (!init)
    {
        fd.f = stderr;
        init = true;
    }

    return fat_cast(fd, mx_file_t, IStream);
}

MX_API fatptr_t(IStream) mx_open(const char *file, mx_open_flags flags)
{
    char options[6] = "b";
    if (flags & MX_OPEN_READ) strcat(options, "r");
    if (flags & MX_OPEN_WRITE) strcat(options, "w");
    if (flags & MX_OPEN_APPEND) strcat(options, "a");
    if (flags & MX_OPEN_NEW) strcat(options, "s");

    mx_file_t *self = malloc(sizeof(mx_file_t));
    if (!self) 
    {
        return fat_new(NULL, *NULL, IStream);
    }

    self->f = fopen(file, options);
    return fat_new(self, fat_vtable(mx_file_t, IStream), IStream);
}
