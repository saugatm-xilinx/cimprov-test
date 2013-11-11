from cfg import TesterConfiguration

TesterConfiguration(
    name=__name__.split('.')[-1],
    desc='Configuration for running on ESXi host',
    vendorNS='solarflare/cimv2',
    interopNS='root/interop',
    isWMI=False,
    classListPath = '../libcimobjects/classes',
    host='http://127.0.0.1:5989',
    user='root', passwd='news@ben')