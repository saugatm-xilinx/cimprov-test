/***************************************************************************//*! \file liprovider/sf_base64.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2014/01/07
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef SOLARFLARE_SF_BASE64_H
#define SOLARFLARE_SF_BASE64_H 1

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
namespace solarflare
{
#endif

    /// Base64 coding table
    static char base64_code_table[] = {
                                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/' };

    ///
    /// Compute size of encoded string for a given binary data size
    ///
    /// @param src_size     Size of binary data to be encoded
    ///
    /// @return Number of bytes to be allocated for the encoded string
    ///
    static inline size_t base64_enc_size(size_t src_size)
    {
        return (src_size / 3 + (src_size % 3 > 0 ? 1 : 0)) * 4 +
               ((3 - src_size % 3) % 3) + 1;
    }

    ///
    /// Compute size of decoded binary data
    ///
    /// @param encoded    Encoded base64 string
    ///
    /// @return Number of bytes to be allocated for the decoded data
    ///
    static inline ssize_t base64_dec_size(const char *encoded)
    {
        size_t len = strlen(encoded);
        size_t padding = 0;

        if (encoded[len - 1] == '=')
            padding++;
        if (encoded[len - 2] == '=')
            padding++;

        if ((len - padding) % 4 != 0)
            return -1;
        return (len - padding) / 4 * 3 - padding;
    }

    ///
    /// Encode binary data in base64 string
    ///
    /// @param dst        Where to store encoded string
    /// @param src        Data to be encoded
    /// @param src_size   Size of data to be encoded
    ///
    static inline void base64_encode(char *dst, const char *src,
                                     size_t src_size)
    {
        int i;
        int j;
        int a;
        int b;
        int c;

        for (i = 0;
             i < ((src_size % 3 == 0) ?
                  (src_size / 3) : (src_size / 3 + 1));
             i++)
        {
            a = src[3 * i];

            if (3 * i + 1 < src_size)
                b = src[3 * i + 1];
            else
                b = 0;

            if (3 * i + 2 < src_size)
                c = src[3 * i + 2];
            else
                c = 0;

            dst[4 * i] = base64_code_table[(a >> 2) & 0x3f];
            dst[4 * i + 1] = base64_code_table[((a << 4) & 0x30)
                                               + ((b >> 4) & 0x0f)];
            dst[4 * i + 2] = base64_code_table[((b << 2) & 0x3c)
                                               + ((c >> 6) & 0x03)];
            dst[4 * i + 3] = base64_code_table[c & 0x3f];
        }

        for (j = 0; j < (3 - src_size % 3) % 3; j++)
            dst[4 * i + j] = '=';

        dst[4 * i + j] = '\0';
    }

    ///
    /// Return index of a symbol in base64 code table
    ///
    /// @param c    Symbol
    ///
    /// @return Symbol index
    ///
    static inline int base64_symbol_id(char c)
    {
        int i;

        for (i = 0; i < sizeof(base64_code_table); i++)
            if (base64_code_table[i] == c)
                return i;

        return -1;
    }

    ///
    /// Decode base64 string
    ///
    /// @param dst    Where to store decoded data
    /// @param src    Encoded string to be decoded
    ///
    /// @return 0 on success, -1 on failure
    ///
    static inline int base64_decode(char *dst, const char *src)
    {
        int i;
        int len = strlen(src);
        int padding = 0;
        int i_max;
        int a;
        int b;
        int c;
        int d;

        if (src[len - 1] == '=')
            padding++;
        if (src[len - 2] == '=')
            padding++;

        if ((len - padding) % 4 != 0)
            return -1;

        i_max = (len - padding) / 4;

        for (i = 0; i < i_max; i++)
        {
            a = base64_symbol_id(src[4 * i]);
            b = base64_symbol_id(src[4 * i + 1]);
            c = base64_symbol_id(src[4 * i + 2]);
            d = base64_symbol_id(src[4 * i + 3]);

            if (a < 0 || b < 0 || c < 0 || d < 0)
                return -1;

            dst[3 * i] = ((a << 2) & 0xfc)
                         + ((b >> 4) & 0x03);
            if (!(i == i_max - 1 && padding >= 2))
                dst[3 * i + 1] = ((b << 4) & 0xf0)
                                 + ((c >> 2) & 0x0f);
            if (!(i == i_max - 1 && padding >= 1))
                dst[3 * i + 2] = ((c << 6) & 0xc0)
                                 + (d & 0x3f);
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif

#endif // SOLARFLARE_SF_BASE64_H
