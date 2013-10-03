#!/bin/bash

RUNDIR="$(pwd -P)"
PYWBEM_PACK_URL="http://downloads.sourceforge.net/pywbem/pywbem-0.7.0.tar.gz"
PYWBEM_PACK_PATH=${PYWBEM_PACK_PATH}

pushd `dirname \`which $0\`` >/dev/null
TESTSUITE_DIR="$(pwd -P)"
popd >/dev/null

TMPDIR=$(mktemp -d /tmp/pywbem.XXX)

exit_cleanup()
{
    rm -rf "${TMPDIR}"
    cd "${RUNDIR}"
    exit 1
}

cd "${TMPDIR}"
if [ -z "${PYWBEM_PACK_PATH}" ]; then
    wget ${PYWBEM_PACK_URL} || exit_cleanup
else
    cp ${PYWBEM_PACK_PATH} . || exit_cleanup
fi
tar -xzf pywbem-0.7.0.tar.gz || exit_cleanup

cd pywbem-0.7.0
if test -e setup.py; then
    python setup.py build || exit_cleanup
    python setup.py install --install-lib=${TESTSUITE_DIR} || exit_cleanup
fi

rm -rf "${TMPDIR}"
cd "${RUNDIR}"
