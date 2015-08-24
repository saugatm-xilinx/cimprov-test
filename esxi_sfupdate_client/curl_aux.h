#ifndef SFUPDATE_CIM_CURL_AUX_H
#define SFUPDATE_CIM_CURL_AUX_H 1

/** Where to store data received from CURL */
extern char *curl_data;
/** Received data length */
extern int curl_data_len;

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
extern size_t curl_write(void *data, size_t size,
                         size_t count, void *ptr);

#endif // SFUPDATE_CIM_CURL_AUX_H
