#include <mx/digest.h>
#include <string.h>

const uint32_t FVN_BASIS = 0x811c9dc5;
const uint32_t FVN_PRIME = 0x01000193;

MX_IMPL int mx_compare_digest(const void* a, const void *b, size_t size)
{
    return memcmp(a, b, size);
}

MX_IMPL void mx_adler32(adler32_t *digest, const char *src, size_t length)
{
    const uint32_t modulo = 65521;
    uint32_t a = 1, b = 0;

    while (length--)
    {
        a = (a + *(src++)) % modulo;
        b = (a + b)        % modulo;
    }

    return (b << 16) | (a << 0);
}

MX_IMPL void mx_fvn0(fvn0_t *digest, const char *src, size_t length)
{
    *digest = 0;

    for (size_t i = 0; i < length; i++)
    {
        *digest *= FVN_PRIME;
        *digest ^= (fvn0_t)src[i];
    }
}

MX_IMPL void mx_fvn1(fvn1_t    *digest, const char *src, size_t length)
{
    *digest = FVN_BASIS;

    for (size_t i = 0; i < length; i++)
    {
        *digest *= FVN_PRIME;
        *digest ^= src[i];
    }
}

MX_IMPL void mx_fvn1a(fvn1a_t   *digest, const char *src, size_t length)
{
    *digest = FVN_BASIS;

    for (size_t i = 0; i < length; i++)
    {
        *digest ^= src[i];
        *digest *= FVN_PRIME;
    }
}