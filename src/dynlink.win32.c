#if _WIN32
#include "mx/dynlink.h"
#include "mx/assert.h"
#include "windows.h"

MX_IMPL mx_lib_t mx_lib_open(const char *name)
{
    HMODULE lib = LoadLibrary(name);
    return (mx_lib_t)lib;
}

MX_IMPL void mx_lib_close(mx_lib_t lib)
{
    MX_ASSERT_PTR(lib, "The library must be open.");
    MX_ASSERT(lib != MX_LIB_SELF, "You cannot close this module.");

    FreeLibrary((HMODULE)lib);
}

MX_IMPL void *mx_lib_get_symbol(mx_lib_t lib, const char *symbol)
{
    MX_ASSERT_PTR(lib, "The library must be open.");

    /* The self library pointer is retreived with GetModuleHandle. */
    if (lib == MX_LIB_SELF)
    {
        lib = (mx_lib_t)GetModuleHandle(NULL);
    }

    return GetProcAddress((HMODULE)lib, symbol);
}

#endif
