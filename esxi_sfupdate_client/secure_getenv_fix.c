#include <stdlib.h>

/*
 *  * This fixes a problem with linking with newer glibc,
 *   * where __secure_getenv() is renamed into secure_getenv().
 *    * Some of the static libraries with which this program links
 *     * expect __secure_getenv().
 *      */
char *__secure_getenv(const char *name)
{
#ifdef SECURE_GETENV_FOUND
    return secure_getenv(name);
#else
    return getenv(name);
#endif
}
