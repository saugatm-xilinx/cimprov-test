#! /bin/sh

CLASSLIST=classes
PRODUCTNAME=Solarflare

./genclass -r -e -F ${CLASSLIST}
./genprov -F${CLASSLIST}
./genmod ${PRODUCTNAME} -F${CLASSLIST}
#genmak -f ${PRODUCTNAME} *.cpp

