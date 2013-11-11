/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_SOURCE>
** \author  cjk
**  \brief  Device state dump format
**   \date  2012/12/21
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/* !!! Only edit this file in the firmwaresrc repo. !!! */


#ifndef MC_DUMP_FORMAT
#define MC_DUMP_FORMAT

#include <stdint.h>

#define DUMP_FORMAT_ALIGN_PAD(addr_) (((addr_) + 3u) & ~3u)


#define DH_NIDENT 16

struct dump_header {
  unsigned char ident[DH_NIDENT];
  uint32_t type;
  uint32_t version; /* increase means incompatible changes */
  uint32_t arch;
  uint32_t dh_size; /* sizeof (struct dump_header) */
  uint32_t sh_offset;
  uint32_t sh_entsize; /* sizeof (struct section_header) */
  uint32_t sh_count;
  uint32_t dumpfile_size;
};

#define DH_IDENT_INIT {'\x7f', 'S', 'F', 'D', 'U', 'M', 'P', 0, 0, 0, 0, 0, 0, 0, 0, 0}

#define DH_TYPE_DUMPFILE 1
#define DH_TYPE_DUMPSPEC 2

#define DH_VERSION_v1 1

#define DH_ARCH_EF10 1


struct section_header {
  uint32_t type;
  uint32_t format;
  uint32_t index;
  uint32_t address_offset;
  uint32_t address_stride;
  uint32_t address_count;
  uint32_t data_offset;
  uint32_t data_size;
};

#define SH_TYPE_DUMP_REASON_CODE           0
#define SH_TYPE_DUMP_REASON                1

#define SH_TYPE_VERSION_HARDWARE_REVISION   8
#define SH_TYPE_VERSION_HARDWARE_BUILD_ID   9
#define SH_TYPE_VERSION_PLATFORM_NAME      10
#define SH_TYPE_VERSION_SERIALNO           11
#define SH_TYPE_VERSION_FIRMWARE_TIMESTAMP 12
#define SH_TYPE_VERSION_FIRMWARE_VERSION   13
#define SH_TYPE_VERSION_FIRMWARE_EXT_INFO  17
#define SH_TYPE_VERSION_PART_NUMBER        18
#define SH_TYPE_VERSION_ENGINEERING_CHANGE 19

#define SH_TYPE_MCPU_REGS                 64
#define SH_TYPE_MCPU_IMEM                 65 /* placeholder, not supported,
                                                look at your flash file */
#define SH_TYPE_MCPU_DMEM                 66
#define SH_TYPE_MCPU_PMEM                 67
#define SH_TYPE_MCPU_DIRECT               68 /* registers/tables on the
                                                MC's direct bus */

#define SH_TYPE_DPCPU_REGS               128
#define SH_TYPE_DPCPU_IMEM               129 /* placeholder, not supported,
                                                look at your flash file */
#define SH_TYPE_DPCPU_DMEM               130
#define SH_TYPE_DPCPU_DIRECT             131 /* registers/tables on the
                                                DPCPU's direct bus */

#define SH_TYPE_DICPU_REGS               160 /* placeholder, not currently
                                                relevant */
#define SH_TYPE_DICPU_IMEM               161 /* placeholder, not supported,
                                                look at your flash file */
#define SH_TYPE_DICPU_DMEM               162
#define SH_TYPE_DICPU_DIRECT             163

#define SH_TYPE_CSR                      224
#define SH_TYPE_LUE                      225
#define SH_TYPE_SMC                      226
#define SH_TYPE_CONFIG                   227
#define SH_TYPE_I2C                      228
#define SH_TYPE_PBMX                     240

/* TBD: suitable list of environmental readings */
#define SH_TYPE_TEMPERATURE_DIE          256
#define SH_TYPE_TEMPERATURE_CORE         257
#define SH_TYPE_TEMPERATURE_REGULATOR    258
#define SH_TYPE_TEMPERATURE_AMBIENT      259
#define SH_TYPE_FANSPEED_QQQ             288
#define SH_TYPE_AIRFLOW_QQQ              320
#define SH_TYPE_VOLTAGE_123V             352
#define SH_TYPE_CURRENT_123V             384


#define SH_FORMAT_DUMP_REASON_CODE     0 /* uint32; values below */
#define SH_FORMAT_ASCIIZ               1 /* nul terminate ascii; can be
                                            nul padded */
#define SH_FORMAT_UNIX_TIMESTAMP       2 /* uint32 */
#define SH_FORMAT_FIRMWARE_VERSION     3 /* uint16 x 4 */
#define SH_FORMAT_UINT8                8
#define SH_FORMAT_UINT16               9
#define SH_FORMAT_UINT32              10
#define SH_FORMAT_UINT64              11
#define SH_FORMAT_UINT128             12
#define SH_FORMAT_UINT256             13
#define SH_FORMAT_UINT512             14
#define SH_FORMAT_REG8                16 /* completeness only */
#define SH_FORMAT_REG16               17
#define SH_FORMAT_REG32               18
#define SH_FORMAT_LUE                 32 /* 146-bit (19 byte) values; 1 byte padding */
#define SH_FORMAT_SMC                 33 /* 72-bit (9 byte) values; 3 bytes padding */
#define SH_FORMAT_PBMX                48 /* 48-bit (6 byte) signals;8 bit (1 byte) time delta; 1 bytes padding */

/* Thesse formats related to devices on the i2c bus, and there
   are therefore likely to be quite a lot.  As the MC needs to
   deteremine data size from the format, it is useful to group
   similar formats to allow this to be done efficiently. It is
   proposed that the following groupings be used:

     64 -  95   1 byte generic formats    -
     96 - 127   1 byte formats specific to some device
    128 - 175   2 byte little endian generic formats
    176 - 223   2 byte bug endian generic formats
    224 - 255   2 byte formats specific to some device
*/
#define SH_FORMAT_TC664_50RPM         96  /* uint8;  speed/50rpm */
#define SH_FORMAT_FLOAT16LE_5_11      128 /* uint16le; 5 bits signed exponent, 11 bits signed mantissa */
#define SH_FORMAT_FIXED16LE_M12       139 /* uint16le; implicit -12 exponent, 16 bits signed mantissa */
#define SH_FORMAT_FIXED16BE_M8        186 /* uint16be; implicit  -8 exponent, 16 bits signed mantissa */


#define SH_FORMAT_DUMP_REASON_CODE_SOLICITED             1
#define SH_FORMAT_DUMP_REASON_CODE_UNSOLICITED           2 /* for debug use only */
#define SH_FORMAT_DUMP_REASON_CODE_UNSOLICITED_ASSERTION 3
#define SH_FORMAT_DUMP_REASON_CODE_UNSOLICITED_WATCHDOG  4
#define SH_FORMAT_DUMP_REASON_CODE_UNSOLICITED_ECC       5


#define SH_FORMAT_DUMP_REASON_CODE_DATA_SIZE (4)

#define SH_FORMAT_UNIX_TIMESTAMP_DATA_SIZE (4)

#define SH_FORMAT_FIRMWARE_VERSION_DATA_SIZE (8)

#define SH_FORMAT_UINT8_DATA_SIZE (1)

#define SH_FORMAT_UINT16_DATA_SIZE (2)

#define SH_FORMAT_UINT32_DATA_SIZE (4)

#define SH_FORMAT_UINT64_DATA_SIZE (8)

#define SH_FORMAT_UINT128_DATA_SIZE (16)

#define SH_FORMAT_UINT256_DATA_SIZE (32)

#define SH_FORMAT_UINT512_DATA_SIZE (64)

#define SH_FORMAT_REGS_MASK_COUNT(n_regs_) \
  (((n_regs_) + 31) / 32)

#define SH_FORMAT_REGS_MASK_DATA_SIZE(n_regs_) \
  (SH_FORMAT_REGS_MASK_COUNT(n_regs_) * SH_FORMAT_UINT32_DATA_SIZE)

#define SH_FORMAT_REG8_DATA_SIZE(n_regs_) \
  (SH_FORMAT_REGS_MASK_DATA_SIZE(n_regs_) + (n_regs_) * SH_FORMAT_UINT8_DATA_SIZE)

#define SH_FORMAT_REG16_DATA_SIZE(n_regs_) \
  (SH_FORMAT_REGS_MASK_DATA_SIZE(n_regs_) + (n_regs_) * SH_FORMAT_UINT16_DATA_SIZE)

#define SH_FORMAT_REG32_DATA_SIZE(n_regs_) \
  (SH_FORMAT_REGS_MASK_DATA_SIZE(n_regs_) + (n_regs_) * SH_FORMAT_UINT32_DATA_SIZE)

#define SH_FORMAT_LUE_DATA_SIZE (20)

#define SH_FORMAT_SMC_DATA_SIZE (12)

#define SH_FORMAT_PBMX_DATA_SIZE (8)

#define SH_INDEX_FORMAT_LBN 28
#define SH_INDEX_FORMAT_WIDTH 4
#define SH_INDEX_FORMAT_NONE 0
#define SH_INDEX_FORMAT_CORE 1
#define SH_INDEX_FORMAT_SATELLITE_CPU 2
#define SH_INDEX_FORMAT_FUNCTION 3
#define SH_INDEX_FORMAT_I2C 4
#define SH_INDEX_FORMAT_PBMX 5

/* Used by CORE, SATELLITE_CPU, I2C, PBMX */
#define SH_INDEX_CORE_NUM_LBN 8
#define SH_INDEX_CORE_NUM_WIDTH 4

/* Used by SATELLITE_CPU */
#define SH_INDEX_SATELLITE_CPU_TXRX_LBN 4
#define SH_INDEX_SATELLITE_CPU_TXRX_WIDTH 1
#define SH_INDEX_SATELLITE_CPU_TXRX_TX 1
#define SH_INDEX_SATELLITE_CPU_TXRX_RX 0

/* Used by SATELLITE_CPU */
#define SH_INDEX_SATELLITE_CPU_CPU_NUM_LBN 0
#define SH_INDEX_SATELLITE_CPU_CPU_NUM_WIDTH 4

/* Used by FUNCTION */
#define SH_INDEX_FUNCTION_PF_NUM_LBN 0
#define SH_INDEX_FUNCTION_PF_NUM_WIDTH 16

/* Used by FUNCTION */
#define SH_INDEX_FUNCTION_VF_NUM_LBN 16
#define SH_INDEX_FUNCTION_VF_NUM_WIDTH 12
#define SH_INDEX_FUNCTION_VF_NUM_NULL 0xfff

/* Used by I2C */
#define SH_INDEX_I2C_ADDR_LBN 16
#define SH_INDEX_I2C_ADDR_WIDTH 8
#define SH_INDEX_I2C_BUS_NUM_LBN 24
#define SH_INDEX_I2C_BUS_NUM_WIDTH 4
#define SH_INDEX_I2C_IS_PMBUS_LBN 15
#define SH_INDEX_I2C_IS_PMBUS_WIDTH 1
#define SH_INDEX_I2C_PMBUS_PAGE_LBN 0
#define SH_INDEX_I2C_PMBUS_PAGE_WIDTH 8

/* Used by PBMX */
#define SH_INDEX_PBMX_BLOCK_NUM_LBN 24
#define SH_INDEX_PBMX_BLOCK_NUM_WIDTH 4
#define SH_INDEX_PBMX_BLOCK_NUM_PCIE 0
#define SH_INDEX_PBMX_BLOCK_NUM_BIU 1
#define SH_INDEX_PBMX_BLOCK_NUM_MC 2
#define SH_INDEX_PBMX_BLOCK_NUM_NWKENG 3
#define SH_INDEX_PBMX_BLOCK_NUM_NWKROUTER 4
#define SH_INDEX_PBMX_BLOCK_NUM_SYSSERV 5
#define SH_INDEX_PBMX_BLOCK_NUM_EV 6
#define SH_INDEX_PBMX_VECTOR_NUM_LBN 16
#define SH_INDEX_PBMX_VECTOR_NUM_WIDTH 8
#define SH_INDEX_PBMX_GROUP_MASK_LBN 0
#define SH_INDEX_PBMX_GROUP_MASK_WIDTH 8

#endif
