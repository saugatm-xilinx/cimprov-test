#!/bin/bash

RUNDIR="$(pwd -P)"

pushd `dirname \`which $0\`` >/dev/null
TESTSUITE_DIR="$(pwd -P)"
mkdir -p "${TESTSUITE_DIR}/pywbem"
popd >/dev/null

TMPDIR=$(mktemp -d /tmp/pywbem.XXX)

exit_cleanup()
{
    rm -rf "${TMPDIR}"
    cd "${RUNDIR}"
    exit 1
}

cd "${TMPDIR}"
wget http://downloads.sourceforge.net/pywbem/pywbem-0.7.0.tar.gz || exit_cleanup
tar -xzf pywbem-0.7.0.tar.gz || exit_cleanup

cd pywbem-0.7.0
if test -e setup.py; then
    python setup.py build || exit_cleanup
    python setup.py install --install-lib=${TESTSUITE_DIR}/pywbem || exit_cleanup
fi

rm -rf "${TMPDIR}"
cd "${RUNDIR}"
