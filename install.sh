#! /bin/sh

NAMESPACE=root/solarflare
PRODUCTNAME=Solarflare

if test "$1" != "upgrade"; then
    REGMOD_OPTS=-c
fi

cp lib${PRODUCTNAME}.so ${PEGASUS_HOME}/lib
regmod -v -n ${NAMESPACE} ${REGMOD_OPTS} ${PEGASUS_HOME}/lib/lib${PRODUCTNAME}.so
