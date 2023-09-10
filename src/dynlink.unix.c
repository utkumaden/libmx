#if __unix__
#include "mx/dynlink.h"
#include "mx/assert.h"
#include "dlfcn.h"

#ifdef DEBUG
    #define RTLD_DEFAULT (RTLD_NOW)
#else
    #define RTLD_DEFAULT (RTLD_LAZY)
#endif

MX_IMPL mx_lib_t mx_lib_open(const char *name)
{
    void *plib = dlopen(name, RTLD_DEFAULT);
    return (mx_lib_t)plib;
}

MX_IMPL void mx_lib_close(mx_lib_t lib)
{
    MX_ASSERT_PTR(lib, "The library must be open.");
    MX_ASSERT(lib != MX_LIB_SELF, "You cannot close this module.");

    dlclose((void*)lib);
}

MX_IMPL void *mx_lib_get_symbol(mx_lib_t lib, const char *symbol)
{
    MX_ASSERT_PTR(lib, "The library must be open.");

    /* The self library pointer is NULL for dlfunc. */
    if (lib == MX_LIB_SELF) lib = NULL;

    return dlsym((void*)lib, symbol);
}

#endif
