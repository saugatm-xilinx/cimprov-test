#!/bin/bash
set -e
. ./solarflare.defs

mkdir -p $DESTDIR
cd $DESTDIR

if [ -e $DESTDIR/etc/cim/openwsman ]; then
   rm -rf $DESTDIR/etc/cim/openwsman
fi
mkdir -p $DESTDIR/etc/cim/openwsman

WSMAN=$(find $PROJDIR -maxdepth 1 -type f -name "*.wsman" -print)
for file in $WSMAN
do
   cp $file  $DESTDIR/etc/cim/openwsman/
done

cd $BUILDDIR
install_local $DESTDIR

cd $DESTDIR
# Remove symlinks for .so files
find $DESTDIR/usr/lib -type l -print | xargs rm -f
# Create provider library file without version extension
LIBFILES=$(find $DESTDIR/usr/lib -type f -name "*.so.*" -print)
for filename in $LIBFILES
do
   basename="${filename%%.so.*}"
   newname=$basename.so
   mv $filename $newname
done
find $DESTDIR/usr/lib -type f -name "*.so" -exec $BINUTILS_DIR/bin/i686-linux5.0-strip '{}' ';'

# Tar provider libraries
tar cf $PKGDIR/$PROJECT-libs.tar usr/lib

# append the openwsman configuration files.
WSMAN=$(find ./etc/cim/openwsman -maxdepth 1 -type f -name "*.wsman" -print)
for file in $WSMAN
do
   tar rf $PKGDIR/$PROJECT-libs.tar $file
done

cd $DESTDIR
gzip -c9 $PKGDIR/$PROJECT-libs.tar > $PKGDIR/$PROJECT-libs.tgz
rm -f $PKGDIR/$PROJECT-libs.tar
echo "Tar file $PKGDIR/$PROJECT-libs.tgz"
tar czf $PKGDIR/$PROJECT-stage.tgz var/lib/sfcb/stage
echo "Tar file $PKGDIR/$PROJECT-stage.tgz"

