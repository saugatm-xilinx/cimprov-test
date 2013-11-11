from cfg import TesterConfiguration

TesterConfiguration(
    name=__name__.split('.')[-1],
    desc='Configuration for running on IBM host',
    vendorNS='root/solarflare',
    interopNS='root/pg_interop',
    isWMI=False,
    classListPath = '../libcimobjects/classes',
    host='http://127.0.0.1:15988',
    user='root', passwd='news@ben')
