
#ifndef _EFTEST_DEVINFO_H_
#define _EFTEST_DEVINFO_H_

#include <ci/compat.h>

enum eftest_arch {
	EFTEST_ARCH_FALCON,
};

typedef struct eftest_devinfo_s {
	int  arch;            /* enum efhw_arch */
	char variant;         /* 'A', 'B', ... */
	int  revision;        /* 0, 1, ... */
	int  in_fpga:1;
	int  in_cosim:1;
} eftest_devinfo_t;

static inline int eftest_devinfo_init(eftest_devinfo_t *dt,
			  int vendor_id, int device_id,
			  int class_revision) 
{
	if (vendor_id != 0x1924)
		return 0;

	memset(dt, 0, sizeof(*dt));
	
	switch (device_id) {
	case 0x0703:
	case 0x6703:
		dt->arch = EFTEST_ARCH_FALCON;
		dt->variant = 'A';
		switch (class_revision) {
		case 0:
			dt->revision = 0;
			break;
		case 1:
			dt->revision = 1;
			break;
		default:
			return 0;
		}
		break;
	case 0x0710:
		dt->arch = EFTEST_ARCH_FALCON;
		dt->variant = 'B';
		switch (class_revision) {
		case 2:
			dt->revision = 0;
			break;
		default:
			return 0;
		}
		break;
	/* Development */
	case 0x0770:
		dt->arch = EFTEST_ARCH_FALCON;
		dt->variant = 'C';
		dt->in_fpga = 1;
		break;
	case 0x7777:
		dt->arch = EFTEST_ARCH_FALCON;
		dt->variant = 'C';
		dt->in_fpga = 1;
		switch (class_revision) {
		case 0:
			dt->revision = 0;
			break;
		default:
			return 0;
		}
		break;
	/* cosim */
	case 0x7778:
		dt->arch = EFTEST_ARCH_FALCON;
		dt->variant = 'C';
		dt->in_cosim = 1;
		dt->revision = 0;
		if (class_revision > 0xf)
			return 0;
		break;
	case 0x0803:
        case 0x0813:
		dt->arch = EFTEST_ARCH_FALCON;
		dt->variant = 'C';
		switch (class_revision) {
		case 0: /* ASIC */
			dt->revision = 0;
			dt->in_fpga = 0;
			break;

		case 1:
		case 2: /* 20/Oct/08 indicates DBI has been alt initialized */
                case 3: /* 30/Sep/08 indicates DBI has been initialized */
			dt->revision = 0;
			dt->in_fpga = 1;
			break;

		default:
			return 0;
		}
		break;
	default:
		return 0;
	}

	return 1;
}

#endif /* _EFTEST_DEVINFO_H_ */

