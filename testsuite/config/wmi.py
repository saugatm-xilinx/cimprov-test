from cfg import TesterConfiguration

TesterConfiguration(
    name=__name__.split('.')[-1],
    desc='Configuration for running on Windows host using WMI client',
    vendorNS='\\\\root\\cimv2',
    interopNS='\\\\root\\cimv2',
    isWMI=True,
    classListPath = 'classes',
    user='Administrator', passwd='news@ben')