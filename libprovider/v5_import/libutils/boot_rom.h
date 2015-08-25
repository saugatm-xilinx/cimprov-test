/*
 * Copyright 2008 Solarflare Communications Inc. All rights reserved.
 * Use is subject to license terms.
 */

#ifndef BOOT_ROM_H
#define BOOT_ROM_H

/*
 * A version string must appear within the first 0x400 bytes of the
 * boot-ROM image and begin with this prefix.
 *
 * Firmware image files have the version number in the header and
 * there is no need to search for the version string in them.
 */

#define BOOT_ROM_VERSION_MAX_OFFSET 0x1000

#define BOOT_ROM_VERSION_PREFIX "Boot Manager (v"

#endif /* BOOT_ROM_H */
