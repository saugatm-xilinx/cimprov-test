#!/bin/bash
set -e
. ./solarflare.defs
[ ! -z "$BORA" ] || export BORA=$PWD/../../bora

mkdir -p $DEPSDIR
pushd $DEPSDIR
if [ "$CIMDE" != "1" ] ; then
   $TAR zxf $PKGDIR/cmpi.tgz
   $TAR zxf $PKGDIR/cmpi-dev.tgz
fi
$TAR zxf $PKGDIR/hwinfo-dev.tgz
$TAR zxf $PKGDIR/hwinfo-libs.tgz
$TAR zxf $PKGDIR/hwinfo-tools.tgz
$TAR zxf $PKGDIR/sfcb-cimom.tgz
$TAR zxf $PKGDIR/sfcb-tools.tgz
$TAR zxf $PKGDIR/openslp-bin.tgz
$TAR zxf $PKGDIR/sfcc.tgz

cd $PROJDIR
acinit

mkdir -p $BUILDDIR
cd $BUILDDIR

export PATH=$DEPSDIR/bin:$PATH
export LD_LIBRARY_PATH=$DEPSDIR/lib:$LD_LIBRARY_PATH

linklib $BUILDDIR/tmp $TCROOT/lin32/libvmkuser-0.11

export CFLAGS="$CC_INC -I$DEPSDIR/include -I$BUILDDIR/tmp/include -I$SFCBDIR -fPIC $DEBUG_CFLAGS -DCMPI_VER_86 -fstack-protector-all -D_FORTIFY_SOURCE=1" 
export LDFLAGS="$CC_LIB -L$DEPSDIR/lib -Wl,-z,relro -Wl,-z,now"
$PROJDIR/configure $DEBUG_CFG \
 --with-sfcb \
 --with-smash-namespace=$OEM_NAMESPACE/cimv2 \
 --with-sfcb-interop-namespace=root/interop \
 --with-config-namespace=$OEM_NAMESPACE/config \
 --with-sfcb-schema-dir=$SCHEMADIR \
 --build=$BUILD_SYSTEM --host=$TARGET_SYSTEM --prefix= --libdir=/usr/lib/cim --bindir=/usr/lib/cim
