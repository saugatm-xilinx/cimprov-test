#! /bin/sh

NAMESPACE=root/solarflare
PRODUCTNAME=Solarflare

cp lib${PRODUCTNAME}.so ${PEGASUS_HOME}/lib
regmod -v -n ${NAMESPACE} -c ${PEGASUS_HOME}/lib/lib${PRODUCTNAME}.so
