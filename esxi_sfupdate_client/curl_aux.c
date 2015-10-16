#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cim_log.h"

/** Where to store data received from CURL */
char *curl_data = NULL;
/** Received data length */
int curl_data_len = 0;

/**
 * Data receiving callback for CURL
 *
 * @param data    Received data pointer
 * @param size    Data member size
 * @param count   Number of members
 * @param ptr     Unused
 *
 * @return Number of bytes stored successfully
 */
size_t curl_write(void *data, size_t size, size_t count, void *ptr)
{
    int nbytes = size * count;

    char *p;
    int   new_len;

    new_len = curl_data_len + nbytes;
    p = realloc(curl_data, new_len);
    if (p == NULL)
    {
        ERROR_MSG("%s(): out of memory", __FUNCTION__);
        return 0;
    }

    curl_data = p;
    memcpy(curl_data + curl_data_len, data, nbytes);
    curl_data_len += nbytes;

    return nbytes;
}
