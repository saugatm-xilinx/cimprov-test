/***************************************************************************//*! \file tools/image_table_gen/test_image.c
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2018/03/28
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*
 * This program takes path to firmware image as its only parameter,
 * and outputs its firmware type and subtype in a format
 * "string_type.numeric_subtype", e.g. FIRMWARE_MCFW.22
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../../libprovider/v5_import/libutils/firmware.h"

/**
 * Convert firmware type to string.
 *
 * @param type    Firmware type number.
 *
 * @return String representation.
 */
const char *type2str(int type)
{
    switch (type)
    {
        case IMAGE_TYPE_BOOTROM:
            return "FIRMWARE_BOOTROM";
        case IMAGE_TYPE_MCFW:
            return "FIRMWARE_MCFW";
	case IMAGE_TYPE_UEFIROM:
            return "FIRMWARE_UEFIROM";
        default:
            return "UNKNOWN";
    }
}

int main(int argc, const char *argv[])
{
    FILE    *f = NULL;
    size_t   fsize;
    ssize_t  rlen;
    uint8_t *buf = NULL;
    int      rc = 1;

    sfupdate_image_t image;

    if (argc < 2)
    {
        fprintf(stderr, "File is not specified\n");
        goto cleanup;
    }

    f = fopen(argv[1], "rb");
    if (f == NULL)
    {
        fprintf(stderr, "Failed to open file '%s'\n",
                argv[1]);
        goto cleanup;
    }

    fseek(f, 0L, SEEK_END);
    fsize = ftell(f);
    rewind(f);

    buf = (uint8_t *)calloc(1, fsize);
    if (buf == NULL)
    {
        fprintf(stderr, "Out of memory\n");
        goto cleanup;
    }

    rlen = fread(buf, 1, fsize, f);
    if (rlen != fsize)
    {
        fprintf(stderr, "Failed to read expected data from image file\n");
        goto cleanup;
    }

    if (init_image_from_buffer(buf, fsize, &image) != 0)
    {
        fprintf(stderr, "init_image_from_buffer() failed\n");
        goto cleanup;
    }

    printf("%s.%u", type2str(image.reflash_header->ih_type),
           (unsigned int)image.reflash_header->ih_subtype);

    rc = 0;

cleanup:

    free(buf);
    if (f != NULL)
        fclose(f);
    return rc;
}
