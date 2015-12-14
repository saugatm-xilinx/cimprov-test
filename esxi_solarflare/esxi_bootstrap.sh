#! /bin/sh

cimpdk_ver=`rpm --queryformat "%{VERSION}" -q vmware-esx-cimpdk-devtools`
cimpdk_v1=`echo $cimpdk_ver | sed "s/^\([0-9]*\).*/\1/"`
cimpdk_v2=`echo $cimpdk_ver | sed "s/^[0-9]*[.]\([0-9]*\).*/\1/"`

if test $cimpdk_v1 -le 5 -a $cimpdk_v2 -le 1 ; then

    PRODUCT_NAME=solarflare
    CIMPDK_ROOT=/opt/vmware/cimpdk
    CIMPDK_SAMPLES=${CIMPDK_ROOT}/samples

    set -e
    set -v

    for sysdir in cim-schema cmpi common hwinfo omc sfcb support; do
        cp -r ${CIMPDK_SAMPLES}/$sysdir .
    done
    chmod -R +w .

    sed -i "s/acme/$PRODUCT_NAME/g" common/visor-oemtgz.sh

    cd solarflare
    set +v 
    source ../common/defs
    set -v
    ln -sf `i686-linux5.0-g++ -print-file-name=libstdc++.a` .

    sed -i 's/__cdecl//g' $OPENSOURCE/sfcb/$SFCB_VER/cmpios.h

else

    CIMPDK_ROOT=`ls /opt/vmware/ | grep cimpdk-$cimpdk_ver`
    CIMPDK_ROOT="/opt/vmware/$CIMPDK_ROOT"
    CIMPDK_SAMPLES=${CIMPDK_ROOT}/samples

    cp $CIMPDK_SAMPLES/acme/Makefile .

    sed -i "s/\${PROJDIR}\/src/\${PROJDIR}\/solarflare/g" Makefile
    sed -i "s/PROV_NAME=acme/PROV_NAME=solarflare-cim-provider/g" Makefile
    sed -i "s/PROV_NAMESPACE=acme/PROV_NAMESPACE=solarflare/g" Makefile
    sed -i "s/DESC_VENDOR=.*/DESC_VENDOR=Solarflare/g" Makefile
    sed -i "s/DESC_VENDOR_EMAIL=.*/DESC_VENDOR_EMAIL=support@solarflare.com/g" Makefile
    sed -i "s/DESC_SUMMARY=.*/DESC_SUMMARY=Solarflare NIC CIM provider/g" Makefile
    sed -i "s/DESC_VERSION=.*/DESC_VERSION=2.0-0.0/g" Makefile
    sed -i "s/DESC_DESCRIPTION=.*/DESC_DESCRIPTION=Solarflare NIC CIM provider for VMware ESXi/g" Makefile
    sed -i "s/DESC_URLS=.*/DESC_URLS=oem\/descriptor-urls.xml/g" Makefile
    sed -i "s/DESC_DEPENDS=.*/DESC_DEPENDS=oem\/solarflare-vib-depends.xml/g" Makefile
    sed -i "s/DESC_PROVIDES=.*/DESC_PROVIDES=oem\/solarflare-vib-provides.xml/g" Makefile
    sed -i "s/libacmeprovider.so/libsolarflare_nic_provider.so/g" Makefile
    sed -i "s/\<CFLAGS=\"\(.*\)/CFLAGS=\"-D__USE_XOPEN2K8=1 \1/g" Makefile
    sed -i "s/\${CURRENT_DIR}\/descriptor.xml/\${CURRENT_DIR}\/oem\/descriptor.xml/g" Makefile
    sed -i "s/\${CURRENT_DIR}\/bulletin.xml/\${CURRENT_DIR}\/oem\/bulletin.xml/g" Makefile
    sed -i "s/BULL_KBURL=.*/BULL_KBURL=none/g" Makefile
    sed -i "s/BULL_VENDOR_CODE=VMW/BULL_VENDOR_CODE=SFL/g" Makefile
    sed -i "s/PROV_VIB_BASENAME=.*$/PROV_VIB_BASENAME=\$(BULL_VENDOR_CODE)-ESX-\${PROV_NAME}-\${DESC_VERSION}/g" Makefile

    if test $cimpdk_v1 -le 5 -a $cimpdk_v2 -le 5 ; then
        MK_CC=`cat Makefile | grep "export\s*CC\s*="`
        MK_CXX=`echo $MK_CC | sed "s/-gcc/-g++/"`
        MK_CXX=`echo $MK_CXX | sed "s/CC/CXX/"`

        echo "" >>Makefile
        echo "$MK_CXX" >>Makefile

        sed -i "s/DESC_LIVE_INSTALL_ALLOWED=.*$/DESC_LIVE_INSTALL_ALLOWED=false/g" Makefile
        sed -i "s/DESC_LIVE_REMOVE_ALLOWED=.*$/DESC_LIVE_REMOVE_ALLOWED=false/g" Makefile
        sed -i "s/DESC_STATELESS_READY=.*$/DESC_STATELESS_READY=false/g" Makefile
        sed -i "s/DESC_CIMOM_RESTART=.*$/DESC_CIMOM_RESTART=false/g" Makefile
    fi

    # This fixes the bug with renaming our provider to li.so
    sed -i "s/\/\\\\.so/\/[.]so/g" Makefile

    echo "" >>Makefile
    echo "copy_static_std_cpp:" >>Makefile
    echo "	\${CP} \`\${CXX} -print-file-name=\"libstdc++.a\"\` \${SRCDIR}" >>Makefile
    make copy_static_std_cpp

    # Using libdir as destination for our provider library
    echo "usrdestdir = \$(libdir)" >solarflare/Makefile_fixed
    cat solarflare/Makefile.am | sed "s/bin_PROGRAMS/usrdest_PROGRAMS/" >>solarflare/Makefile_fixed
    mv solarflare/Makefile_fixed solarflare/Makefile.am
fi
