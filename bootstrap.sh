#! /bin/sh

CLASSLIST=classes
PRODUCTNAME=Solarflare

genproj -e ${PRODUCTNAME} -F${CLASSLIST}
#genmak -f ${PRODUCTNAME} *.cpp

