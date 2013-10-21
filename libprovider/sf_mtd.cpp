/***************************************************************************//*! \file liprovider/sf_mtd.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Be aware - this file is mainly copy-paste from v5-incoming

#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include <sys/utsname.h>
#include <unistd.h>

#include "efx_ioctl.h"
#include "ci/mgmt/mc_driver_pcol.h"
#include "ci/mgmt/mc_dynamic_cfg.h"

#include <cimple.h>
#include <cimple/Strings.h>
#include <cimple/Mutex.h>
#include <cimple/Auto_Mutex.h>

#include "sf_utils.h"

#define DEV_PATH_MAX 256

#ifndef MTD_CHAR_MJAOR
#define MTD_CHAR_MAJOR 90
#endif
#ifndef MTD_BLOCK_MAJOR
#define MTD_BLOCK_MAJOR 31
#endif

#define NV_BLOCK_SIZE 4096

#define PART_NAME_MAX (IFNAMSIZ + 20) /* matches sfc_mtd */

#define VARLIBDIR "/var/lib/sfcim"

namespace solarflare 
{
    using cimple::Mutex;
    using cimple::Auto_Mutex;
    using cimple::String;

    static size_t cur_nv_part_size;
    static unsigned cur_dev_num;
    static char cur_part_name[PART_NAME_MAX];
    static int cur_fd = -1;
    static bool mtd_is_block;
    static int cur_bootrom_subtype;

    enum nv_part_type {
      NV_PART_MCFW,
      NV_PART_DYNAMIC_CFG,
      NV_PART_BOOTROM,
      NV_PART_BOOTROM_CFG,
      NV_PART_PHY,
      NV_PART_FPGA,
      NV_PART_FCFW,
      NV_PART_CPLD,
      NV_PART_FPGA_BACKUP,
      NV_PART_LICENSE,
    };

    static char *mtd_part_name(char *part_name, const char *netif_name,
                               enum nv_part_type type, unsigned subtype)
    {
        const char *type_name;

        switch (type)
        {
            case NV_PART_MCFW:
                type_name = "sfc_mcfw";
                break;
            case NV_PART_DYNAMIC_CFG:
                type_name = "sfc_dynamic_cfg";
                break;
            case NV_PART_BOOTROM:
                type_name = "sfc_exp_rom";
                break;
            case NV_PART_BOOTROM_CFG:
                type_name = "sfc_exp_rom_cfg";
                break;
            case NV_PART_PHY:
                type_name = "sfc_phy_fw";
                break;
            case NV_PART_FPGA:
                type_name = "sfc_fpga";
                break;
            case NV_PART_FCFW:
                type_name = "sfc_fcfw";
                break;
            case NV_PART_CPLD:
                type_name = "sfc_cpld";
                break;
            case NV_PART_FPGA_BACKUP:
                type_name = "sfc_fpgadiag";
                break;
            case NV_PART_LICENSE:
                type_name = "sfc_license";
                break;
            default:
                return NULL;
        }

        snprintf(part_name, PART_NAME_MAX, "%s %s:%02x",
                 netif_name, type_name, subtype);

        return part_name;
    }

#define KERNEL_VERSION_4(a,b,c,d) \
    (((a) << 24) + ((b) << 16) + ((c) << 8) + (d))

    static int linux_version_code_4(void)
    {
        struct utsname utsname;
        unsigned version, patchlevel, sublevel = 0, stablepatch = 0;

        if (uname(&utsname))
            return -1;
        if (sscanf(utsname.release, "%u.%u.%u.%u",
                   &version, &patchlevel,
                   &sublevel, &stablepatch) < 2)
            return -1;
        return KERNEL_VERSION_4(version, patchlevel,
                                sublevel, stablepatch);
    }

    static int
    mtd_find(const char *name, unsigned *dev_num, size_t *nv_part_size)
    {
        FILE *mtd_list;
        char line[256];
        const char* dev_name = NULL;
        unsigned long int part_size = 0, erase_size = 0;
        char *part_name;
        bool found = false;
        int error;

        /* Look for boot code partition in /proc/mtd.  This job would be
         * a lot easier if MTD used the standard device model!
         */
        mtd_list = fopen("/proc/mtd", "r");
        if (!mtd_list)
            return -1;

        /* Ignore the header line */
        fgets(line, sizeof(line), mtd_list);

        errno = 0;
        while (fgets(line, sizeof(line), mtd_list))
        {
            /* Parse each following line, which should have a device name,
             * partition size, erase size and partition name in the format:
             * "%s: %08x %08x \"%s\""
             */
            char* p;
            dev_name = line;
            p = strchr(line, ':');
            if (!p)
                break;
            *p++ = 0; /* terminate device name */
            part_size = strtoul(p, &p, 16);
            if (*p != ' ')
                break;
            erase_size = strtoul(p, &p, 16);
            if (*p != ' ')
                break;
            p += strspn(p, " ");
            if (*p != '"')
                break;
            part_name = p + 1;
            p = strchr(part_name, '"');
            if (!p)
                break;
            *p = 0; /* terminate partition name */

            /* Check the partition name */
            if (strcmp(part_name, name) == 0)
            {
                found = true;
                break;
            }
        }
        error = errno;

        fclose(mtd_list);

        if (found)
        {
            if (dev_num)
            {
                char* end;
                assert(strncmp(dev_name, "mtd", 3) == 0);
                *dev_num = strtoul(dev_name + 3, &end, 10);
                assert(*end == 0);
            }
            if (nv_part_size)
                *nv_part_size = part_size;
        }

        errno = error ? error : found ? 0 : ENOENT;
        return found ? 0 : -1;
    }

    static bool mtd_exists(const char *netif_name,
                           enum nv_part_type type, unsigned subtype)
    {
      char part_name[PART_NAME_MAX];
      return (mtd_part_name(part_name, netif_name, type, subtype) &&
              mtd_find(part_name, NULL, NULL) == 0);
    }

    static int mtd_reopen()
    {
        static bool probed_mtdchar = false, probed_mtdblock = false,
                    create_dev_nodes = false;
        static bool buggy_mtdblock = false,
                    reported_buggy_mtdblock = false;
        int kernel_version;
        unsigned dev_num;
        char dev_path[DEV_PATH_MAX];
        int wait;

        if (cur_fd >= 0)
            close(cur_fd);

        if (mtd_find(cur_part_name, &dev_num, &cur_nv_part_size))
            return -1;

        wait = probed_mtdchar ? 0 : 5;
        for ( ; ; )
        {
            if (create_dev_nodes)
            {
                sprintf(dev_path, "%s/dev_mtd%u", VARLIBDIR, dev_num);
                if (mkdir(VARLIBDIR, 0755) && errno != EEXIST)
                    break;
                if (mknod(dev_path, S_IFCHR | 0600,
                          makedev(MTD_CHAR_MAJOR, 2 * dev_num)) &&
                    errno != EEXIST)
                    break;
            }
            else
                sprintf(dev_path, "/dev/mtd%u", dev_num);

            cur_fd = open(dev_path, O_RDONLY);
            if (cur_fd >= 0)
            {
                mtd_is_block = false;
                return 0;
            }

            if (errno != ENOENT && errno != ENODEV && errno != ENXIO)
                return -1;

            /* If MTD device nodes aren't created in /dev,
             * try creating/using
             * them in our directory under /var/lib. */
            if (errno == ENOENT && probed_mtdchar &&
                !create_dev_nodes && wait == 0)
            {
                create_dev_nodes = true;
                continue;
            }

            if (wait-- == 0)
                break;

            /* The mtdchar module may not be automatically loaded. */
            if (!probed_mtdchar)
            {
                probed_mtdchar = true;
                if (system("modprobe mtdchar >/dev/null 2>&1") &&
                    system("/sbin/modprobe mtdchar >/dev/null 2>&1"))
                    break;
            }

            sleep(1);
        }

        /* Fall back to mtdblock if mtdchar is not available.  Only try to
         * load the mtdblock module if we know it is hotplug-safe.  In
         * kernel versions before 2.6.17 it will trigger an oops in
         * sysfs_remove_dir() when an MTD is removed.  In kernel versions
         * 2.6.26 and 2.6.27 (before 2.6.27.53) the bdi sysfs code will
         * trigger an oops if an mtdblock device is removed and re-added.
         */
        kernel_version = linux_version_code_4();
        if (kernel_version < KERNEL_VERSION_4(2,6,17,0) ||
            (kernel_version >= KERNEL_VERSION_4(2,6,26,0) &&
             kernel_version < KERNEL_VERSION_4(2,6,27,53)))
        {
            buggy_mtdblock = true;
            probed_mtdblock = true;
        }

        wait = probed_mtdblock ? 0 : 5;
        for ( ; ; )
        {
            if (create_dev_nodes)
            {
                sprintf(dev_path, "%s/dev_mtdblock%u", VARLIBDIR, dev_num);
                if (mkdir(VARLIBDIR, 0755) && errno != EEXIST)
                    break;
                if (mknod(dev_path, S_IFBLK | 0600,
                          makedev(MTD_BLOCK_MAJOR, dev_num))
                    && errno != EEXIST )
                    break;
            }
            else
                sprintf(dev_path, "/dev/mtdblock%u", dev_num);

            cur_fd = open(dev_path, O_RDONLY);
            if (cur_fd >= 0)
            {
                mtd_is_block = true;
                return 0;
            }

            if (errno != ENOENT && errno != ENODEV && errno != ENXIO)
            {
                CIMPLE_ERR(("%s(): obtained bad errno %d (%s)",
                            __FUNCTION__, errno, strerror(errno)));
                return -1;
            }

            if (errno == ENOENT && probed_mtdblock && !create_dev_nodes &&
                wait == 0 )
            {
                create_dev_nodes = true;
                continue;
            }

            if (wait-- == 0)
                break;

            if(!probed_mtdblock)
            {
                probed_mtdblock = true;
                if (system("modprobe mtdblock >/dev/null 2>&1") &&
                    system("/sbin/modprobe mtdblock >/dev/null 2>&1"))
                  break;
            }

            sleep(1);
        }

        if (buggy_mtdblock && !reported_buggy_mtdblock)
        {
            CIMPLE_ERR(("This command requires either the 'mtdchar' "
                        "or 'mtdblock' driver for access to flash "
                        "memory on Solarstorm adapters.  The current "
                        "kernel does not have the 'mtdchar' driver, but "
                        "may have the 'mtdblock' driver. You may try "
                        "to use the 'mtdblock' driver by running "
                        "'modprobe mtdblock' and then running this "
                        "command again, but you MUST reboot when you "
                        "have finished."));
            reported_buggy_mtdblock = true;
        }

        return -1;
    }

    static int mtd_open(const char *netif_name,
                        enum nv_part_type type, unsigned subtype)
    {
        if (!mtd_part_name(cur_part_name, netif_name, type, subtype))
            return -1;
        return mtd_reopen();
    }

    static void mtd_close()
    {
        close(cur_fd);
        cur_fd = -1;
    }

    static ssize_t
    mtd_read(void *buf, size_t length, off_t offset)
    {
        ssize_t total_bytes = 0;
        uint32_t chunk_size = NV_BLOCK_SIZE;
        uint32_t addr;

        lseek(cur_fd, offset, SEEK_SET);

        for (addr = 0; addr < length; addr += chunk_size)
        {
            ssize_t len;

            if (chunk_size > length - addr)
                chunk_size = length - addr;

            len = read(cur_fd, (char *)buf + addr, chunk_size);
            if (len == -1)
                return -1;
            total_bytes += len;
        }

        return total_bytes;
    }

    int mtdGetBootROMVersion(const char *netif_name, VersionInfo &ver)
    {
        static Mutex lock(false);
        Auto_Mutex   guard(lock);

        char      *configBuf = NULL;
        ssize_t    rc;

        siena_mc_dynamic_config_hdr_t   *header;
        const siena_mc_fw_version_t     *item;

        if (mtd_open(netif_name, NV_PART_DYNAMIC_CFG,
                     0) != 0)
        {
            CIMPLE_ERR(("%s(): failed to open dynamic configuration "
                        "of %s port for reading", __FUNCTION__,
                        netif_name));
            return -1;
        }

        configBuf = new char[cur_nv_part_size];

        if ((rc = mtd_read(configBuf,
                           cur_nv_part_size, 0)) !=
                                          (ssize_t)cur_nv_part_size)
        {
            CIMPLE_ERR(("%s(): mtd_read() returned %ld instead of %ld",
                        __FUNCTION__,
                        static_cast<long int>(rc),
                        static_cast<long int>(cur_nv_part_size)));
            delete[] configBuf;
            mtd_close();
            return -1;
        }

        mtd_close();
        header = (siena_mc_dynamic_config_hdr_t *)configBuf;
        item = &header->fw_version[MC_CMD_NVRAM_TYPE_EXP_ROM];

        ver = VersionInfo(item->version_w, item->version_x,
                          item->version_y, item->version_z);
        delete[] configBuf;
        return 0;
    }
}
