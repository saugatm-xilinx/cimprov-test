#include <stdlib.h>
#include <stdio.h>

#ifndef SFUPDATE_CIM_LOG_H
#define SFUPDATE_CIM_LOG_H 1

/* Maximum error message length */
#define MAX_ERR_STR 1024

/**
 * Print error message with file name and line number
 * specified.
 *
 * @param s_    Format string
 * @param ...   Arguments
 */
#define ERROR_MSG(s_...) \
    do {                                                    \
        char str_[MAX_ERR_STR];                             \
        snprintf(str_, MAX_ERR_STR, s_);                    \
        fprintf(stderr, "File %s, Line %d: %s\n",           \
                __FILE__, __LINE__, str_);                  \
    } while (0)

/**
 * Print error message without file name and line number
 * specified.
 *
 * @param s_    Format string
 * @param ...   Arguments
 */
#define ERROR_MSG_PLAIN(s_...) \
    do {                                                    \
        char str_[MAX_ERR_STR];                             \
        snprintf(str_, MAX_ERR_STR, s_);                    \
        fprintf(stderr, "%s\n", str_);                      \
    } while (0)

#endif // SFUPDATE_CIM_LOG_H
