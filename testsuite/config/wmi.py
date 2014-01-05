from cfg import TesterConfiguration

TesterConfiguration(
    name=__name__.split('.')[-1],
    desc='Configuration for running on Windows host using WMI client',
    vendorNS='\\\\root\\solarflare',
    interopNS='\\\\root\\solarflare',
    isWMI=True,
    classListPath = 'classes',
    user='Administrator', passwd='news@ben')