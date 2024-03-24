#include "mx/io/stream.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "mx/assert.h"

MX_API void IStream_printf(fatptr_t(IStream) str, const char *format, ...)
{
    va_list va;
    va_start(va, format);
    IStream_vprintf(str, format, va);
    va_end(va);
}

MX_API void IStream_vprintf(fatptr_t(IStream) str, const char *format, va_list va)
{
    char buffer[260];
    va_list va2;
    
    va_copy(va2, va);
    int size = vsnprintf(buffer, sizeof buffer, format, va2);
    va_end(va2);

    if (size < sizeof(buffer))
    {
        IStream_write(str, buffer, size);
        return;
    }

    char *buffer2 = malloc(size+1);
    MX_ASSERT_OOM(buffer2);

    va_copy(va2, va);
    vsnprintf(buffer2, size+1, format, va2);
    IStream_write(str, buffer2, size);
    free(buffer2);
    va_end(va2);
}
