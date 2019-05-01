/*
 * Copyright (c) 2006-2016 Solarflare Communications Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the FreeBSD Project.
 */

#ifndef	_SYS_EFX_H
#define	_SYS_EFX_H

#include "efsys.h"
#include "errno.h"
#include "ef10_signed_image_layout.h"

typedef __success(return == 0) int efx_rc_t;

#define EFX_STATIC_ASSERT(_cond)                \
        ((void)sizeof (char[(_cond) ? 1 : -1]))

#define EFX_ARRAY_SIZE(_array)                  \
        (sizeof (_array) / sizeof ((_array)[0]))

#define EFX_FIELD_OFFSET(_type, _field)         \
        ((size_t)&(((_type *)0)->_field))



/*
 * Image header used in unsigned and signed image layouts (see SF-102785-PS).
 *
 * NOTE:
 * The image header format is extensible. However, older drivers require an
 * exact match of image header version and header length when validating and
 * writing firmware images.
 *
 * To avoid breaking backward compatibility, we use the upper bits of the
 * controller version fields to contain an extra version number used for
 * combined bootROM and UEFI ROM images on EF10 and later (to hold the UEFI ROM
 * version). See bug39254 and SF-102785-PS for details.
 */
typedef struct efx_image_header_s {
	uint32_t	eih_magic;
	uint32_t	eih_version;
	uint32_t	eih_type;
	uint32_t	eih_subtype;
	uint32_t	eih_code_size;
	uint32_t	eih_size;
	union {
		uint32_t	eih_controller_version_min;
		struct {
			uint16_t	eih_controller_version_min_short;
			uint8_t		eih_extra_version_a;
			uint8_t		eih_extra_version_b;
		};
	};
	union {
		uint32_t	eih_controller_version_max;
		struct {
			uint16_t	eih_controller_version_max_short;
			uint8_t		eih_extra_version_c;
			uint8_t		eih_extra_version_d;
		};
	};
	uint16_t	eih_code_version_a;
	uint16_t	eih_code_version_b;
	uint16_t	eih_code_version_c;
	uint16_t	eih_code_version_d;
} efx_image_header_t;

#define	EFX_IMAGE_HEADER_SIZE		(40)
#define	EFX_IMAGE_HEADER_VERSION	(4)
#define	EFX_IMAGE_HEADER_MAGIC		(0x106F1A5)


typedef struct efx_image_trailer_s {
	uint32_t	eit_crc;
} efx_image_trailer_t;

#define	EFX_IMAGE_TRAILER_SIZE		(4)

typedef enum efx_image_format_e {
	EFX_IMAGE_FORMAT_NO_IMAGE,
	EFX_IMAGE_FORMAT_INVALID,
	EFX_IMAGE_FORMAT_UNSIGNED,
	EFX_IMAGE_FORMAT_SIGNED,
        EFX_IMAGE_FORMAT_SIGNED_PACKAGE
} efx_image_format_t;

typedef struct efx_image_info_s {
	efx_image_format_t	eii_format;
	uint8_t *		eii_imagep;
	size_t			eii_image_size;
	efx_image_header_t *	eii_headerp;
} efx_image_info_t;

extern	__checkReturn	efx_rc_t
efx_check_reflash_image(
	__in		void			*bufferp,
	__in		uint32_t		buffer_size,
	__out		efx_image_info_t	*infop);

extern	__checkReturn	efx_rc_t
efx_build_signed_image_write_buffer(
	__out		uint8_t			*bufferp,
	__in		uint32_t		buffer_size,
	__in		efx_image_info_t	*infop,
	__out		efx_image_header_t	**headerpp);

#endif
