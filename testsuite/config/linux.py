from cfg import TesterConfiguration

TesterConfiguration(
    name=__name__.split('.')[-1],
    desc='Default configuration for Linux with Pegasus',
    vendorNS='root/solarflare',
    interopNS='root/pg_interop',
    isWMI=False,
    classListPath = '../libcimobjects/classes')

