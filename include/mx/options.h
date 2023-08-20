#ifndef _MX_GETOPTS_H_
#define _MX_GETOPTS_H_

/**
 * @file options.h MX Library Options Parser
 * This is a very basic options parser built into my applications. It does what
 * getopts.h does on GNU except it's a whole lot less bloated. Simple is the
 * best.
 *
 * It works in two modes, pointer to structure mode and static mode. Static mode
 * works like the old unsafe, not thread safe, not reentrant strtok and familiar
 * static functions. Pointer mode allows you to pass the structure itself.
 * Pointer mode functions are postfixed with `_r`.
 *
 * @example mx_options_static Static Mode Usage.
 * @code{c}
 * #include <stdio.h>
 * #include <mx/options.h>
 *
 * int main(int argc, char** argv)
 * {
 *   mx_optkind_t kind;
 *   mx_options_begin(MX_OPT_DEFAULT, argc, argv);
 *   while (kind = mx_options_next())
 *   {
 *     switch (kind)
 *     {
 *     case MX_OPTION_PAIR:
 *       printf("key:%s value:%s\n", mx_option_key, mx_option_value);
 *       break;
 *     default:
 *       printf("key:%s\n", mx_option_key);
 *       break;
 *     }
 *   }
 * }
 * @endcode
 *
 * @example mx_options_pointer Pointer Mode Usage
 * @code{c}
 * #include <stdio.h>
 * #include <mx/options.h>
 *
 * int main(int argc, char** argv)
 * {
 *   mx_options_t opt;
 *   mx_optkind_t kind;
 *   mx_options_begin_r(&opt, MX_OPT_DEFAULT, argc, argv);
 *   while (kind = mx_options_next_r(&opt))
 *   {
 *     switch (kind)
 *     {
 *     case MX_OPTION_PAIR:
 *       printf("key:%s value:%s\n", opt.key, opt.value);
 *       break;
 *     default:
 *       printf("key:%s\n", opt.key);
 *       break;
 *     }
 *   }
 *
 *   mx_options_end_r(&opt); // Free any resources allocated.
 * }
 * @endcode
 */

#include <mx/base.h>

/**
 * Option kind.
 */
typedef enum mx_optkind_t
{
    MX_OPT_END,             /**< End iterator. */
    MX_OPT_POSITIONAL,      /**< Positional. */
    MX_OPT_SHORT,           /**< (UNIX ONLY) Short flag(s). */
    MX_OPT_LONG,            /**< Long flag. */
    MX_OPT_PAIR,            /**< Key with value.*/
    MX_OPT_DASH,            /**< (UNIX ONLY) A single dash. */
    MX_OPT_DDASH,           /**< (UNIX ONLY) A double dash. */
} mx_optkind_t;

/**
 * Option parse flags.
 */
typedef enum mx_optflag_t
{
    MX_OPT_UNIX = 1 << 0,         /**< Parse UNIX style options. [-h, --help, --include=a]*/
    MX_OPT_DOS  = 1 << 1,         /**< Parse DOS style options. [/?, /help, /include:a] */
    /** Default option style is both. */
    MX_OPT_DEFAULT = MX_OPT_UNIX | MX_OPT_DOS, 
} mx_optflag_t;

/**
 * Option parse storage struct.
 */
typedef struct mx_options_t
{
    int          argc;  /**< Number of arguments. */
    const char **argv;  /**< Array of arguments. */
    mx_optflag_t flags; /**< Parse flags. */
    int          i;     /**< Internal iterator. */
    mx_optkind_t kind;  /**< Last option kind. */
    const char  *key;   /**< Last key. */
    const char  *value; /**< Last value. */
} mx_options_t;

/**
 * Begin parsing options with structure.
 * @param[in] self Storage.
 * @param[in] flags Parse flags.
 * @param[in] argc Number of arguments.
 * @param[in] argv Array of arguments.
 */
MX_API void mx_options_begin_r(mx_options_t *self, mx_optflag_t flags, int argc, const char **argv);

/**
 * Next token.
 * @param[in] self Storage.
 * @returns Option kind.
 */
MX_API mx_optkind_t mx_options_next_r(mx_options_t *self);

/**
 * Stop using the parser.
 * @param[in] self Storage.
 */
MX_API void mx_options_end_r(mx_options_t *self);

/**
 * Internal function that returns a pointer to a static options struct.
 */
MX_INLINE mx_options_t* __mx_options_ptr()
{
    static mx_options_t opts = { 0 };
    return &opts;
}

/**
 * Begin parsing options.
 * @param[in] flags Parse flags.
 * @param[in] argc Number of arguments.
 * @param[in] argv Array of arguments.
*/
MX_INLINE void mx_options_begin(mx_optflag_t flags, int argc, char **argv)
{
    mx_options_end_r(__mx_options_ptr());
    mx_options_begin_r(__mx_options_ptr(), flags, argc, argv);
}

/**
 * Next token.
 * @returns Option kind.
 */
MX_INLINE mx_optkind_t  mx_options_next()
{
    return mx_options_next_r(__mx_options_ptr());
}

/**
 * Current option value.
 */
#define mx_option_key   (__mx_options_ptr()->key)

/**
 * Current option key.
 */
#define mx_option_value (__mx_options_ptr()->value)

#endif