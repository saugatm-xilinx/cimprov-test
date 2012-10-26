#! /bin/sh

NAMESPACE=root/solarflare
INTEROP_NAMESPACE=root/pg_interop
PRODUCTNAME=Solarflare

if test "$1" != "upgrade"; then
    REGMOD_OPTS=-c
fi

cp lib${PRODUCTNAME}.so ${PEGASUS_HOME}/lib
regmod -v -n ${NAMESPACE} ${REGMOD_OPTS} ${PEGASUS_HOME}/lib/lib${PRODUCTNAME}.so
regmod -v -n ${INTEROP_NAMESPACE} ${REGMOD_OPTS} ${PEGASUS_HOME}/lib/lib${PRODUCTNAME}.so SF_RegisteredProfile SF_ReferencedProfile SF_ElementConformsToProfile
