#include <mx/options.h>
#include <mx/assert.h>
#include <stdlib.h>
#include <string.h>

MX_INLINE char* mx_strdup(const char **pptr, char *nstr, size_t len)
{
    if (len == 0)
        len = strlen(nstr);

    char *ptr = realloc(*pptr, len + 1);
    MX_ASSERT_OOM(ptr);

    memcpy(ptr, nstr, len);
    ptr[len] = '\0';

    return (*pptr = ptr);
}

MX_IMPL void mx_options_begin_r(mx_options_t *self, mx_optflag_t flags, int argc, const char **argv)
{
    MX_ASSERT_SELFPTR(self);
    MX_ASSERT_PTR(argv, "Argument array must be non-null.");
    MX_ASSERT(argc > 0, "Arguments count must be greater than zero.");

    self->flags = flags;
    self->argv  = argv;
    self->argc  = argc;
    self->i     = 0;
    self->kind  = MX_OPT_END;
    self->key   = NULL;
    self->value = NULL;
}

MX_IMPL mx_optkind_t mx_options_next_r(mx_options_t *self)
{
    MX_ASSERT_SELFPTR(self);

    if (self->i >= self->argc)
    {
        return (self->kind = MX_OPT_END);
    }

    char  *current = self->argv[self->i];
    char  *next    = (self->i + 1 < self->argc) ? self->argv[self->i + 1] : NULL;
    size_t clen    = strlen(current);

    if ((self->flags & MX_OPT_UNIX) && current[0] == '-')
    {
        /* - indicates a UNIX flag sequence. */

        if (current[1] == '-')
        {
            if (current[2] == '\0')
            {
                /* -- */
                self->i++;
                return (self->kind = MX_OPT_DDASH);
            }
            else
            {
                /* --long-flag or --key=pair*/

                char *equals = strchr(current, '=');

                if (equals)
                {
                    /* The equals is in this token. */
                    size_t len = (size_t)(equals - &current[2]);
                    mx_strdup(&self->key, &current[2], len);

                    if (equals[1] == '\0')
                    {
                        /* The value is in the next token. */
                        next = next ? next : "";
                        mx_strdup(&self->value, next, 0);

                        self->i++;
                    }
                    else
                    {
                        /* The value is also in this token.*/
                        mx_strdup(&self->value, &equals[1], 0);
                    }

                    self->i++;
                    return (self->kind = MX_OPT_PAIR);
                }
                else if (next && next[0] == '=')
                {
                    /* The equals is in the next token. */
                    mx_strdup(&self->key,   &current[2], clen-2);
                    mx_strdup(&self->value, &next[1], 0);

                    self->i += 2;
                    return (self->kind = MX_OPT_PAIR);
                }

                /* Don't treat this as a key pair. It's a long flag.*/
                mx_strdup(&self->key, current, clen-2);
                self->i++;
                return (self->kind = MX_OPT_LONG);
            }
        }
        else if (current[1] == '\0')
        {
            self->i++;
            return (self->kind = MX_OPT_DASH);
        }
        else
        {
            mx_strdup(&self->key,&current[1], clen - 1);
            self->i++;
            return (self->kind = MX_OPT_SHORT);
        }
    }

    if ((self->flags & MX_OPT_DOS) && current[0] == '/')
    {
        /* / indicated a DOS flag sequence. */

        char *colon = strchr(current, ':');

        if (colon)
        {
            /* The colon is in this token. */
            if (colon[1] == '\0')
            {
                /* The value is in the next token. */
                next = next ? next : "";
                mx_strdup(&self->value, next, 0);
                self->i++;
            }
            else
            {
                /* The value is in this token. */
                mx_strdup(&self->value, &colon[1], 0);
            }

            self->i++;
            mx_strdup(&self->key, &current[1], (size_t)(colon-&current[1]));
            return (self->kind = MX_OPT_PAIR);
        }
        else if (next && next[0]==':')
        {
            /* The colon is in the next token. */
            mx_strdup(&self->key, &current[1], clen-1);
            mx_strdup(&self->value, &next[1], 0);
            self->i+=2;
            return (self->kind = MX_OPT_PAIR);
        }
        else
        {
            self->i++;
            mx_strdup(&self->key, &current[1], clen - 1);
            return (self->kind = MX_OPT_LONG);
        }
    }

    /* Otherwise treate as a positional argument. */
    {
        char *ptr = realloc(self->value, clen);
        MX_ASSERT_OOM(ptr);

        self->i++;
        self->value = memcpy(ptr, current, clen);
        return (self->kind = MX_OPT_POSITIONAL);
    }
}

MX_IMPL void mx_options_end_r(mx_options_t *self)
{
    if (self)
    {
        free(self->key);
        free(self->value);
    }
}