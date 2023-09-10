#ifndef _MX_DYNLINK_H_
#define _MX_DYNLINK_H_
/**
 * @file dynlink.h LibMX Dynamic Linker
 *
 * The LibMX dynamic linker is intended to wrap over the native operating system
 * functionalities normally provided by their respective user libraries.
 * Internally dlfunc (*NIX) or LoadLibrary (Win32) is used.
 */

#include "mx/base.h"

/** Handle to module. */
typedef void *mx_lib_t;

/** Handle to own module. */
#define MX_LIB_SELF ((mx_lib_t)~0ll)

/**
 * Open a library.
 * @param[in] name The name of the library.
 * @return Handle to the library. Check against NULL.
 */
MX_API mx_lib_t mx_lib_open(const char *name);

/**
 * Close an already open library.
 * @param[in] lib The library to close.
 */
MX_API void mx_lib_close(mx_lib_t lib);

/**
 * Get the address of a symbol. (a global variable or function.)
 * @param[in] lib The library to look up.
 * @param[in] symbol The name of the symbol to look up.
 * @return Address to the storage location for the symbol.
 */
MX_API void *mx_lib_get_symbol(mx_lib_t lib, const char *symbol);

#endif