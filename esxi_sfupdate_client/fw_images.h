#include <stdlib.h>
#include <stdint.h>

#ifndef SOLARFLARE_FW_IMAGES_H
#define SOLARFLARE_FW_IMAGES_H

typedef struct image_header_s {
    unsigned int    ih_magic;
    unsigned int    ih_version;
    unsigned int    ih_type;
    unsigned int    ih_subtype;
    unsigned int    ih_code_size;
    unsigned int    ih_size;
    unsigned int    ih_controller_version_min;
    unsigned int    ih_controller_version_max;
    unsigned short  ih_code_version_a;
    unsigned short  ih_code_version_b;
    unsigned short  ih_code_version_c;
    unsigned short  ih_code_version_d;
} image_header_t;

typedef struct fw_img_descr {
    uint8_t       *data;
    size_t         size;
} fw_img_descr;

#endif // SOLARFLARE_FW_IMAGES_H
