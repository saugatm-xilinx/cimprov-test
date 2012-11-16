#! /bin/sh

CIMPDK_ROOT=/opt/vmware/cimpdk
CIMPDK_SAMPLES=${CIMPDK_ROOT}/samples
PRODUCT_NAME=solarflare

set -e
set -v

for sysdir in cim-schema cmpi common hwinfo omc sfcb support; do
    cp -r ${CIMPDK_SAMPLES}/$sysdir .
done

sed -i "s/acme/$PRODUCT_NAME/g" common/visor-oemtgz.sh

cd solarflare
set +v 
source ../common/defs
set -v
ln -sf `i686-linux5.0-g++ -print-file-name=libstdc++.a` .

sed -i 's/__cdecl//g' $OPENSOURCE/sfcb/$SFCB_VER/cmpios.h